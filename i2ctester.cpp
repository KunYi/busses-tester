//
// Copyright (C) Microsoft. All rights reserved.
//
#include <mbed.h>
#include "i2ctester.h"

DigitalOut activityLed(LED1);
DigitalOut errorLed(LED2);

void FatalError ()
{
    for (;;) {
        errorLed = 1;
        wait_ms(1000);
        errorLed = 0;
        wait_ms(1000);
    }
}

//
// Delay for the specified interval while blinking the activity LED
//
void BlinkDelay ( uint32_t Milliseconds )
{
    Timer timer;
    timer.start();

    int end = timer.read_ms() + Milliseconds;
    int nextToggle = 0;
    uint32_t i = 0;

    int now;
    while ((now = timer.read_ms()) < end) {
        if (now >= nextToggle) {
            activityLed = ++i & 1;
            nextToggle += 500;
        }
    }

    activityLed = 0;
}

void I2cTester::DelayCurrentHoldMillis ( )
{
    uint32_t timeInMillis =
        (storage[REG_SCL_HOLD_MILLIS_HI] << 8) |
        storage[REG_SCL_HOLD_MILLIS_LO];
    BlinkDelay(timeInMillis);
}

//
// Initialize I2C1 in slave mode on P0.0 (SDA) and P0.1 (SCL)
//
void I2cTester::Init (uint32_t SlaveAddress)
{
    i2c_t device = { LPC_I2C1 };
    i2c_init(&device, P0_0, P0_1);
    i2c_frequency(&device, 100000);

    LPC_I2C1->I2CONCLR = I2C_I2CONCLR_I2ENC;
    LPC_I2C1->I2ADR0 = SlaveAddress << 1;
    LPC_I2C1->I2MASK0 = 0;

    LPC_I2C1->I2CONCLR = I2C_I2CONCLR_STAC | I2C_I2CONCLR_STOC | I2C_I2CONCLR_SIC;
    LPC_I2C1->I2CONSET = I2C_I2CONSET_I2EN | I2C_I2CONSET_AA;
}

void I2cTester::RunStateMachine ( )
{
    switch (LPC_I2C1->I2STAT) {
    // All Master
    case I2C_I2STAT_M_TX_START:     // sent start condition
    case I2C_I2STAT_M_TX_RESTART:   // sent repeated start condition
    case I2C_I2STAT_M_TX_SLAW_ACK:  // slave receiver acked address
    case I2C_I2STAT_M_TX_DAT_ACK:   // slave receiver acked data
    case I2C_I2STAT_M_TX_SLAW_NACK: // address sent, nack received
    case I2C_I2STAT_M_TX_DAT_NACK:  // data sent, nack received
    case I2C_I2STAT_M_TX_ARB_LOST:  // lost bus arbitration
    case I2C_I2STAT_M_RX_DAT_ACK:   // data received, ack sent
    case I2C_I2STAT_M_RX_SLAR_ACK:  // address sent, ack received
    case I2C_I2STAT_M_RX_DAT_NACK:  // data received, nack sent
    case I2C_I2STAT_M_RX_SLAR_NACK: // address sent, nack received
      FatalError();
      break;

    // Slave Receiver
    case I2C_I2STAT_S_RX_SLAW_ACK:              // addressed, returned ack
    case I2C_I2STAT_S_RX_GENCALL_ACK:           // addressed generally, returned ack
    case I2C_I2STAT_S_RX_ARB_LOST_M_SLA:        // lost arbitration, returned ack
    case I2C_I2STAT_S_RX_ARB_LOST_M_GENCALL:    // lost arbitration, returned ack
        activityLed = 1;
        this->AddressedForWrite();
        break;
    case I2C_I2STAT_S_RX_PRE_SLA_DAT_ACK:       // data received, returned ack
    case I2C_I2STAT_S_RX_PRE_GENCALL_DAT_ACK:   // data received generally, returned ack
        activityLed = 1;
        this->ByteReceived(LPC_I2C1->I2DAT);
        break;
    case I2C_I2STAT_S_RX_STA_STO_SLVREC_SLVTRX: // stop or repeated start condition received
        activityLed = 1;
        this->StopReceived();
        this->ReleaseBus();
        break;
    case I2C_I2STAT_S_RX_PRE_SLA_DAT_NACK:      // data received, returned nack
    case I2C_I2STAT_S_RX_PRE_GENCALL_DAT_NACK:  // data received generally, returned nack
        // nack back at master
        this->Nack();
        // ack future responses and leave slave receiver state
        this->ReleaseBus();
        break;
    // Slave Transmitter
    case I2C_I2STAT_S_TX_SLAR_ACK:              // addressed, returned ack
    case I2C_I2STAT_S_TX_ARB_LOST_M_SLA:        // arbitration lost, returned ack
        this->ByteRequested(true);
        activityLed = 1;
        break;
    case I2C_I2STAT_S_TX_DAT_ACK:               // byte sent, ack returned
        this->ByteRequested(false);
        activityLed = 1;
        break;
    case I2C_I2STAT_S_TX_DAT_NACK:              // received nack, we are done
        this->Ack();
        break;
    case I2C_I2STAT_S_TX_LAST_DAT_ACK:          // received ack, but we are done already!
        // ack future responses
        this->Ack();
        break;
    case I2C_I2STAT_BUS_ERROR:                  // bus error, illegal stop/start
        this->Stop();
        break;

    // All
    case I2C_I2STAT_NO_INF:                     // no state information
    default:
        break;
  }

  activityLed = 0;
}

void I2cTester::AddressedForWrite ( )
{
    this->isFirstByte = true;

    // if NAK is armed, check if NAK length is 0, or set up NAK state
    if (this->storage[REG_NAK_CONTROL] != 0xff) {
        if (this->storage[REG_NAK_CONTROL] == 0) {
            this->Nack();
            this->state &= ~STATE_NAK_WRITE;
        } else {
            this->Ack();
            this->countdown = storage[REG_NAK_CONTROL];
            this->state |= STATE_NAK_WRITE;
        }
        // NAK is always one-shot. This resets NAK control.
        this->storage[REG_NAK_CONTROL] = 0xff;
    } else if (this->storage[REG_HOLD_WRITE_CONTROL] != 0xff) {
        if (this->storage[REG_HOLD_WRITE_CONTROL] == 0) {
            this->DelayCurrentHoldMillis();
            this->state &= STATE_HOLD_WRITE;
        } else {
            // enter hold write state
            this->countdown = this->storage[REG_HOLD_WRITE_CONTROL];
            this->state |= STATE_HOLD_WRITE;
        }

        this->Ack();

        // Hold write is always one-shot. This resets hold write.
        this->storage[REG_HOLD_WRITE_CONTROL] = 0xff;
    } else if (this->storage[REG_DISABLE_REPEATED_STARTS] != 0) {
        this->Ack();

        // if disable repeated starts is armed, set the NO_RS flag
        this->state |= STATE_NO_RS;

        // disabling repeated start is always a one-shot operation
        this->storage[REG_DISABLE_REPEATED_STARTS] = 0;
    } else {
        this->Ack();

        // if write NAK or HOLD_WRITE are not armed, ensure we
        // are not in those states
        this->state &= ~(STATE_HOLD_WRITE | STATE_NAK_WRITE | STATE_NO_RS);
    }
}

void I2cTester::ByteReceived ( uint8_t data )
{
    if (this->state & STATE_NAK_WRITE) {
        // count down until it's time to NAK.
        // zero-length NAK would have been handled in AddressedForWrite
        if (--(this->countdown) == 0) {
            // NAKing here means the next byte that the master sends will
            // be NAKed.
            this->Nack();
            this->state &= ~STATE_NAK_WRITE;
        } else {
            this->Ack();
        }
    } else if (this->state & STATE_HOLD_WRITE) {
        if (--(this->countdown) == 0) {
            this->DelayCurrentHoldMillis();
            this->state &= ~STATE_HOLD_WRITE;
        }

        // data received in HOLD_WRITE mode is ignored

        this->Ack();
    } else {
        this->Ack();

        if (this->isFirstByte) {
            this->isFirstByte = false;
            this->address = data;
            return;
        }

        // if we're in eeprom range, increment and wrap address
        if (this->address <= EEPROM_ADDRESS_MAX) {
            this->storage[this->address] = data;
            this->address = (this->address + 1) % (EEPROM_ADDRESS_MAX + 1);
            return;
        }

        // update register with data. address does not auto increment
        // for register writes
        switch (this->address) {
        case REG_SCL_HOLD_MILLIS_HI:
            // increment address so that both HI and LO bytes can
            // be written in a single operation
            this->storage[this->address] = data;
            ++(this->address);
            break;
        case REG_DISABLE_REPEATED_STARTS:
        case REG_SCL_HOLD_MILLIS_LO:
        case REG_HOLD_READ_CONTROL:
        case REG_HOLD_WRITE_CONTROL:
        case REG_NAK_CONTROL:
            this->storage[this->address] = data;
            break;
        case REG_CHECKSUM_UPDATE:
        {
            uint32_t crc = this->checksum.Update(data);
            this->storage[REG_CHECKSUM_UPDATE] = uint8_t((crc >> 8) & 0xff);
            this->storage[REG_CHECKSUM_RESET] =  uint8_t(crc & 0xff);
            break;
        }
        case REG_CHECKSUM_RESET:
            this->checksum.Reset();
            this->storage[REG_CHECKSUM_UPDATE] = 0;
            this->storage[REG_CHECKSUM_RESET] = 0;
            break;
        default:
            // writes to reserved registers are ignored
            break;
        }
    }
}

void I2cTester::ByteRequested ( bool isStart )
{
    if (isStart && (this->storage[REG_HOLD_READ_CONTROL] != 0xff)) {
        if (this->storage[REG_HOLD_READ_CONTROL] == 0) {
            this->countdown = 0;
            this->DelayCurrentHoldMillis();
        } else {
            // enter the hold read state
            this->countdown = this->storage[REG_HOLD_READ_CONTROL];
            this->state |= STATE_HOLD_READ;
        }

        LPC_I2C1->I2DAT = this->countdown;
        this->Ack();

        // ensure that hold read is a one-shot operation
        this->storage[REG_HOLD_READ_CONTROL] = 0xff;
    } else if (this->state & STATE_HOLD_READ) {
        // hold read state counts down until it's time to hold
        if (--(this->countdown) == 0) {
            this->DelayCurrentHoldMillis();
        }

        LPC_I2C1->I2DAT = this->countdown;
        this->Ack();
    } else {
        // all other states return current register value
        LPC_I2C1->I2DAT = this->storage[this->address];
        this->Ack();

        if (this->address <= EEPROM_ADDRESS_MAX) {
            // addresses within the eeprom increment and wrap
            this->address = (this->address + 1) % (EEPROM_ADDRESS_MAX + 1);
        } else {
            // address increments and wraps
            ++(this->address);
        }
    }
}

void I2cTester::StopReceived ( )
{
    // check if we should fail repeated starts
    if (this->state & STATE_NO_RS) {
        this->Stop();

        // disabling repeated starts is always a one-shot operation
        this->state &= ~STATE_NO_RS;
    }
}
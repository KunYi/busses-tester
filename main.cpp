/*
    Copyright(c) Microsoft Open Technologies, Inc. All rights reserved.

    The MIT License(MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files(the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions :

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

//
// busses-tester: Firmware for the mbed LPC1768 that complements the
// Windows Devices Low Level HLK tests.
//

#include <mbed.h>
#include "i2cdef.h"

#define SLAVE_ADDR 0x55

void i2c_addressed_for_write ( );
void i2c_byte_received ( uint8_t data );
void i2c_byte_requested ( bool isStart );
void i2c_stop_received ( );

DigitalOut activityLed(LED1);
DigitalOut errorLed(LED2);

class Crc16
{
public:

    Crc16 ( ) : crc(0) { }

    uint32_t Get ( ) const { return this->crc & 0xffff; }

    uint32_t Update ( uint8_t data )
    {
        this->crc = (this->crc << 8) ^
            crc16table[((this->crc >> 8) ^ data) & 0xff];
        return this->Get();
    }

    void Reset ( ) { this->crc = 0; }

private:
    static const uint16_t crc16table[];

    uint32_t crc;
};

// CRC16 table for CCITT polynomial (0x1021)
const uint16_t Crc16::crc16table[] = {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

class I2cTester
{
public:

    I2cTester () :
        state(STATE_NORMAL),
        address(0),
        countdown(0),
        isFirstByte(false)
    { }

    //
    // Initialize I2C1 in slave mode on P0.0 (SDA) and P0.1 (SCL)
    //
    static void Init (uint32_t SlaveAddress);

    //
    // Call this in a loop to run the I2C state machine
    //
    void RunStateMachine ( );

private:

    enum REGISTERS {
        EEPROM_ADDRESS_MAX = 0x7F,
        REG_DISABLE_REPEATED_STARTS = 0xF8,
        REG_SCL_HOLD_MILLIS_HI = 0xF9,
        REG_SCL_HOLD_MILLIS_LO = 0xFA,
        REG_HOLD_READ_CONTROL = 0xFB,
        REG_HOLD_WRITE_CONTROL = 0xFC,
        REG_NAK_CONTROL = 0xFD,
        REG_CHECKSUM_UPDATE = 0xFE,
        REG_CHECKSUM_RESET = 0xFF,
    };

    enum : uint16_t {
        SCL_HOLD_DEFAULT = 15000
    };

    enum STATE : uint32_t {
        STATE_NORMAL = 0,
        STATE_NAK_WRITE = 0x1,  // NAK_WRITE, HOLD_WRITE and NO_RS are mutually exclusive
        STATE_HOLD_WRITE = 0x2,
        STATE_NO_RS = 0x4,
        STATE_HOLD_READ = 0x8,  // HOLD_READ can be combined with NAK_WRITE or HOLD_WRITE
    };

    static void Ack ( )
    {
        LPC_I2C1->I2CONCLR = I2C_I2CONCLR_SIC;
        LPC_I2C1->I2CONSET = I2C_I2CONSET_AA;
    }

    static void Nack ( )
    {
        LPC_I2C1->I2CONCLR = I2C_I2CONCLR_SIC | I2C_I2CONCLR_AAC;
    }

    static void Stop ( )
    {
        LPC_I2C1->I2CONCLR = I2C_I2CONCLR_SIC;
        LPC_I2C1->I2CONSET = I2C_I2CONSET_STO | I2C_I2CONSET_AA ;
    }

    static void ReleaseBus ( )
    {
        LPC_I2C1->I2CONCLR = I2C_I2CONCLR_STAC | I2C_I2CONCLR_STOC | I2C_I2CONCLR_SIC;
        LPC_I2C1->I2CONSET = I2C_I2CONSET_I2EN | I2C_I2CONSET_AA;
    }

    void AddressedForWrite ( );
    void ByteReceived ( uint8_t data );
    void ByteRequested ( bool isStart );
    void StopReceived ( );

    void DelayCurrentHoldMillis ( );

    int state;              // bitwise OR of STATE values
    uint32_t address;       // current EEPROM address
    Crc16 checksum;         // current checksum
    uint8_t countdown;      // counts down number of bytes until special operation
    bool isFirstByte;

    // provide 256 bytes of storage in our virtual eeprom
    uint8_t storage[256];
};

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
    uint8_t i = 0;

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
        // zero-length NAK would have been handled in i2c_addressed_for_write
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

I2cTester i2cTester;

int main ()
{
    i2cTester.Init(SLAVE_ADDR);
    for (;;) i2cTester.RunStateMachine();
    return 0;
}

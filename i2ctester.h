//
// Copyright (C) Microsoft. All rights reserved.
//
#ifndef _I2CTESTER_H_
#define _I2CTESTER_H_

namespace Lldt {
namespace I2c {

class I2cTester
{
public:

    I2cTester () :
        state(STATE_NORMAL),
        address(0),
        countdown(0),
        isFirstByte(false)
    {
        this->storage[REG_VERSION] = VERSION;
    }

    //
    // Initialize I2C1 in slave mode on P0.0 (SDA) and P0.1 (SCL)
    //
    static void Init ( );

    //
    // Call this in a loop to run the I2C state machine
    //
    void RunStateMachine ( );

private:

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
    Crc16 checksum;         // current checksum
    uint8_t address;        // current EEPROM address
    uint8_t countdown;      // counts down number of bytes until special operation
    bool isFirstByte;
    uint8_t storage[256];   // provide 256 bytes of storage in our virtual eeprom
};

} // namespace I2c
} // namespace Lldt

#endif // _I2CTESTER_H_

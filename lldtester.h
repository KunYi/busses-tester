//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Low Level Devices Tester Interface
//
// This file must remain compatible with both MSVC and GCC.
//

#ifndef _LLDTESTER_H_
#define _LLDTESTER_H_

#include <stdint.h>

namespace Lldt {
namespace I2c {

enum {
    //
    // The slave address of the test device
    //
    SLAVE_ADDRESS = 0x55,

    //
    // Version of the interface. Increment this when a breaking change is made
    // to the interface.
    //
    VERSION = 1,

    NACK_INDEX_MAX = 0xFE,
    STRETCH_INDEX_MAX = 0xFE,
};

enum REGISTERS {
    EEPROM_ADDRESS_MAX = 0x7F,
    REG_VERSION = 0xF7,
    REG_DISABLE_REPEATED_STARTS = 0xF8,
    REG_SCL_HOLD_MILLIS_HI = 0xF9,
    REG_SCL_HOLD_MILLIS_LO = 0xFA,
    REG_HOLD_READ_CONTROL = 0xFB,
    REG_HOLD_WRITE_CONTROL = 0xFC,
    REG_NAK_CONTROL = 0xFD,
    REG_CHECKSUM_UPDATE = 0xFE,
    REG_CHECKSUM_RESET = 0xFF,
};

} // namespace I2c
namespace Spi {

enum : uint32_t {
    //
    // Device signature returned in DeviceId field of TesterInfo.
    //
    DEVICE_ID = 0x7B216A38,

    //
    // Version of the interface. Increment this when a breaking change is made
    // to the interface.
    //
    VERSION = 1,
};

//
// Commands understood by the tester.
//
enum SpiTesterCommand {
    Invalid,
    GetDeviceInfo = 0x81,
    CaptureNextTransfer,
    GetTransferInfo,
};

//
// Status codes for clock active time measurement.
//
enum class ClockMeasurementStatus {
    Success,
    UnknownError,
    EdgeNotDetected,
    Overflow,
};

//
// SPI data modes.
//
enum SpiDataMode {
    Mode0,
    Mode1,
    Mode2,
    Mode3,
};

//
// SPI connection settings to use for communicating with the SPI tester
// control interface.
//
const SpiDataMode SPI_CONTROL_INTERFACE_MODE = Mode3;
const uint32_t SPI_CONTROL_INTERFACE_FREQUENCY = 4000000;
const uint32_t SPI_CONTROL_INTERFACE_DATABITLENGTH = 8;

#pragma pack(push,1)

//
// Describes the test device's capabilities.
//
struct TesterInfo {
    //
    // Signature of the test device (DEVICE_ID).
    //
    uint32_t DeviceId;

    //
    // Contains the interface version number that the tester was built against.
    //
    uint32_t Version;

    //
    // The maximum SPI clock frequency at which the test device can operate.
    //
    uint32_t MaxFrequency;

    //
    // The number of ticks per second of the counter used to measure
    // ClockActiveTime.
    //
    uint32_t ClockMeasurementFrequency;

    //
    // The minimum data bit length supported by the tester.
    //
    uint8_t MinDataBitLength;

    //
    // The maximum data bit length supported by the tester.
    //
    uint8_t MaxDataBitLength;
};

//
// Contains information about a captured transfer.
//
struct TransferInfo {
    //
    // The CRC16 of all bytes received by the tester in capture mode.
    //
    uint32_t Checksum;

    //
    // The number of elements received by the tester in capture mode.
    //
    uint32_t ElementCount;

    //
    // The index of the first out-of-sequence element received in capture
    // mode, or ElementCount if no out-of-sequence elements were detected.
    //
    uint32_t MismatchIndex;

    //
    // Status code indicating if the clock active time was successfully measured.
    //
    ClockMeasurementStatus ClockActiveTimeStatus;

    //
    // The number of ticks (in units of ClockMeasurementFrequency ticks per
    // second) between the first falling edge of SCK and the last falling
    // edge of SCK.
    //
    uint32_t ClockActiveTime;

};

//
// Represents the binary format of a command sent to the test device.
//
struct CommandBlock {
    CommandBlock (SpiTesterCommand Command_ = SpiTesterCommand::Invalid) :
        Command(uint8_t(Command_)),
        u()
    { }

    uint8_t Command;    // SpiTesterCommand
    union {
        struct {
            uint8_t Mode;               // SpiDataMode
            uint8_t DataBitLength;

            //
            // The initial value that the tester should expect the master
            // to send
            //
            uint16_t SendValue;

            //
            // The initial value that the tester should send to the master.
            //
            uint16_t ReceiveValue;
        } CaptureNextTransfer;

        uint8_t RawBytes[7];
    } u;
};

#pragma pack(pop) // pack(push,1)

static_assert(
    sizeof(CommandBlock) == 8,
    "SpiTester command block must be 8 bytes");

} // namespace Spi
} // namespace Lldt

#endif // _LLDTESTER_H_

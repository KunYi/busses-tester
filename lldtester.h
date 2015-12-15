//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Low Level Devices Tester Interface
//
// This file must remain compatible with both MSVC and GCC.
//

#ifndef _LLDTESTER_H_
#define _LLDTESTER_H_

#include <cstdint>
#include <climits>

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
    VERSION = 2,
};

//
// Commands understood by the tester.
//
enum SpiTesterCommand {
    Invalid,
    GetDeviceInfo = 0x81,
    CaptureNextTransfer,
    GetTransferInfo,
    StartPeriodicInterrupts,
    AcknowledgeInterrupt,
    GetPeriodicInterruptInfo,
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

enum : uint32_t { INVALID_TIME_SINCE_FALLING_EDGE = 0xffffffffUL };

//
// SPI connection settings to use for communicating with the SPI tester
// control interface.
//
const SpiDataMode SPI_CONTROL_INTERFACE_MODE = Mode3;
const uint32_t SPI_CONTROL_INTERFACE_FREQUENCY = 4000000;
const uint32_t SPI_CONTROL_INTERFACE_DATABITLENGTH = 8;

#pragma pack(push,1)

//
// This structure is prepended to all SPI transactions from the test device
// to the master (that is, when the master does a READ operation). This
// structure contains information that the master can use to verify that the
// payload received from the slave device is valid.
//
struct TransferHeader {
    struct {
        //
        // The CRC16 of all bytes in the transfer, including this header, with
        // this field zeroed out.
        //
        uint16_t Checksum;

        //
        // The total length of the transfer including this header.
        //
        uint16_t Length;
    } Header;
};

//
// Describes the test device's capabilities.
//
struct TesterInfo : public TransferHeader {
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
struct TransferInfo : public TransferHeader {
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
    // Status code indicating if the clock active time was successfully
    // measured.
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
// Bitfield structure indicating possible errors that can occur in
// periodic interrupt mode.
//
union PeriodicInterruptStatus {
    struct {
        //
        // An invalid command was received while in periodic interrupt mode.
        // The only command that is valid in periodic interrupt mode is the
        // AcknowledgeInterrupt command. Any other command will cause
        // an exit from periodic interrupt mode.
        //
        uint32_t NotAcknowledged : 1;

        //
        // The chip select line was deasserted before the entire
        // AcknowledgeInterrupt command could be received. This error will
        // cause periodic interrupt mode to be exited.
        //
        uint32_t IncompleteReceive : 1;

        //
        // The chip select line was deasserted before all response data to the
        // AcknowledgeInterrupt command could be sent. This error will not
        // cause periodic interrupt to be exited.
        //
        uint32_t IncompleteTransmit : 1;

        //
        // The transmit FIFO could not be serviced in time while transmitting
        // data, meaning that data read by the master is likely invalid. This
        // error does not cause an exit from periodic interrupt mode.
        //
        uint32_t TransmitUnderrun : 1;

        //
        // The specified duration and frequency would result in a total number
        // of interrupts that would exceed the range of a uint32_t. Use a lower
        // frequency or duration.
        //
        uint32_t ArithmeticOverflow : 1;

        uint32_t reserved : 27;
    } s;
    uint32_t AsUInt32;
};

static_assert(
    sizeof(PeriodicInterruptStatus) == sizeof(uint32_t),
    "Verifying size of PeriodicInterruptStatus bitfield");

//
// Output of the AcknowledgeInterrupt command.
// The AcknowledgeInterrupt command should be sent as a write-read command,
// where the write portion of the transfer is a CommandBlock, and the
// read portion of the transfer is an AcknowledgeInterruptInfo structure.
//
struct AcknowledgeInterruptInfo  {
    //
    // The number of ticks (in units of ClockMeasurementFrequency ticks per
    // second) that elapsed since the interrupt pin was asserted to the first
    // falling edge of SCK since the interrupt. If TimeSinceFallingEdge is
    // INVALID_TIME_SINCE_FALLING_EDGE it means the interrupt was already
    // acknowledged, and the latency calculation would be invalid.
    //
    uint32_t TimeSinceFallingEdge;

    //
    // This will be set to the bitwise NOT of TimeSinceFallingEdge.
    //
    uint32_t Checksum;

    //
    // Returns true if checksum validation succeeds.
    //
    bool ChecksumValid ( ) const
    {
        return this->TimeSinceFallingEdge == ~this->Checksum;
    }

    //
    // Returns true if the interrupt was already acknowledged. If the
    // interrupt was already acknowledged, the latency calculation is not valid
    // and TimeSinceFallingEdge should be ignored.
    //
    bool AlreadyAcknowledged ( ) const
    {
        return this->TimeSinceFallingEdge == INVALID_TIME_SINCE_FALLING_EDGE;
    }
};

//
// Output of the GetPeriodicInterruptInfo command. This command should be sent
// after exiting periodic interrupt mode to retrieve statistics about the
// just-ended periodic interrupt session.
//
struct PeriodicInterruptInfo : public TransferHeader {
    //
    // Indicates any errors that occurred while in periodic interrupt mode.
    // This status is cumulative and will contain all errors that occurred
    // during periodic interrupt mode.
    //
    PeriodicInterruptStatus Status;

    //
    // Total number of interrupt events that occurred during periodic interrupt
    // mode. This number does not necessarily equal the number of falling edges
    // generated; there may be fewer falling edges than total interrupts if
    // an interrupt was not acknowledged in time.
    //
    uint32_t InterruptCount;

    //
    // The number of interrupts that were acknowledged before the next
    // interrupt was generated.
    //
    uint32_t AcknowledgedBeforeDeadlineCount;

    //
    // The number of interrupts that were acknowledged after the next interrupt
    // was generated.
    //
    uint32_t AcknowledgedAfterDeadlineCount;

    //
    // The number of times an acknowledgement was received for an interrupt
    // that had already been acknowledged.
    //
    uint32_t AlreadyAcknowledgedCount;

    //
    // The number of interrupts that were dropped. This would correspond
    // to the number of dropped samples for an A/D converter.
    //
    uint32_t DroppedInterruptCount ( ) const
    {
        return this->InterruptCount - this->AcknowledgedBeforeDeadlineCount -
            this->AcknowledgedAfterDeadlineCount;
    }

    //
    // The total number of successful acknowledgements received.
    //
    uint32_t TotalAcknowledgeCount ( ) const
    {
        return this->AcknowledgedBeforeDeadlineCount +
            this->AcknowledgedAfterDeadlineCount;
    }
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

        struct {
            //
            // The rate at which to generate interrupts. A falling edge will
            // be produced on the interrupt pin this many times per second.
            //
            uint32_t InterruptFrequency;

            //
            // How long to generate interrupts in seconds. The total number of
            // interrupts generated will be
            // InterruptFrequency * DurationInSeconds
            //
            uint16_t DurationInSeconds;

            //
            // Computes the total number of interrupts that will be generated
            // into the output parameter, and returns true if the calculation
            // is valid. If false is returned, an arithmetic overflow has
            // occurred and InterruptFrequency or DurationInSeconds needs
            // to be reduced.
            //
            bool ComputeInterruptCount (uint32_t& InterruptCount) const
            {
                uint64_t count64 = uint64_t(this->InterruptFrequency) *
                    this->DurationInSeconds;
                InterruptCount = static_cast<uint32_t>(count64);

                // Cannot use std::numeric_limits<uint32_t>::max() since
                // 'max' collides with a macro on Windows
                return count64 <= ULONG_MAX;
            }
        } StartPeriodicInterrupts;

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

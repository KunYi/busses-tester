//
// Copyright (C) Microsoft. All rights reserved.
//
#ifndef _SPITESTER_H_
#define _SPITESTER_H_

namespace Lldt {
namespace Spi {

class SpiTester
{
    enum { MIN_DATA_BIT_LENGTH = 4, MAX_DATA_BIT_LENGTH = 16 };

public:

    void Init ( );

    void RunStateMachine ( );

private:

    static void SspInit ();

    static void SspSetDataMode (SpiDataMode Mode, uint32_t DataBitLength);

    static void SspSendImpl (TransferHeader& Data);

    template <typename Ty>
    static void SspSendWithChecksum (Ty& Data)
    {
        static_assert(
            __is_base_of(TransferHeader, Ty),
            "Invalid cast: Ty does not derive from TransferHeader");

        Data.Header.Length = sizeof(Data);
        SspSendImpl(Data);
    }

    static bool ChipSelectAsserted ()
    {
        return (LPC_GPIO0->FIOPIN & (1 << 16)) == 0;
    }

    static void WaitForCsToDeassert ();

    static void TimerInit ();

    static bool ReceiveCommand (CommandBlock& Command);

    static Lldt::Spi::ClockMeasurementStatus WaitForCapture (uint32_t* Capture);

    static Lldt::Spi::TransferInfo CaptureTransfer (const CommandBlock& Command);

    static uint32_t dummy;

    PeriodicInterruptInfo RunPeriodicInterrupts (const CommandBlock& Command);

    TesterInfo testerInfo;
    TransferInfo transferInfo;
    PeriodicInterruptInfo interruptInfo;

public:
    static volatile uint32_t remainingInterrupts;
};

} // namespace Spi
} // namespace LLdt

#endif // _SPITESTER_H_

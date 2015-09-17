//
// Copyright (C) Microsoft. All rights reserved.
//
#ifndef _SPITESTER_H_
#define _SPITESTER_H_

#include <stdint.h>
#include "lldtester.h"

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
    
    static void SspSendWithChecksum (const uint8_t* Data, uint32_t LengthInBytes);

    template <typename T>
    static void SspSendWithChecksum (const T& Data)
    {
        SspSendWithChecksum(reinterpret_cast<const uint8_t*>(&Data), sizeof(Data));
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
    
    TesterInfo testerInfo;
    TransferInfo transferInfo;
};

} // namespace Spi
} // namespace LLdt

#endif // _SPITESTER_H_

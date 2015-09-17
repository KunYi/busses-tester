//
// Copyright (C) Microsoft. All rights reserved.
//
// busses-tester: Firmware for the mbed LPC1768 that complements the
// Windows Devices Low Level HLK tests.
//

//#include <mbed.h>

#include "lldtester.h"

#include "i2ctester.h"
#include "spitester.h"

enum : uint32_t { SLAVE_ADDRESS = 0x55 };

I2cTester i2cTester;
Lldt::Spi::SpiTester spiTester;

int main ()
{
    i2cTester.Init(SLAVE_ADDRESS);
    spiTester.Init();
    
    for (;;) {
        //i2cTester.RunStateMachine();
        spiTester.RunStateMachine();
    }
    
    return 0;
}

//
// Copyright (C) Microsoft. All rights reserved.
//
// busses-tester: Firmware for the mbed LPC1768 that complements the
// Windows Devices Low Level HLK tests.
//
#include <stdint.h>
#include <lpc17xx.h>

#include "util.h"
#include "Lpc17xxHardware.h"
#include "lldtester.h"
#include "i2ctester.h"
#include "spitester.h"

Lldt::I2c::I2cTester i2cTester;
Lldt::Spi::SpiTester spiTester;

int main ()
{
    SetDefaultTimer(LPC_TIM3);

    i2cTester.Init();
    spiTester.Init();

    for (;;) {
        i2cTester.RunStateMachine();
        spiTester.RunStateMachine();
    }

    return 0;
}

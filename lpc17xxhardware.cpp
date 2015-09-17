//
// Copyright (C) Microsoft. All rights reserved.
//
#include <lpc17xx.h>
#include "Lpc17xxHardware.h"

void SetPeripheralPowerState (CLKPWR_PCONP Peripheral, bool Enable)
{
    if (Enable) {
        LPC_SC->PCONP |= Peripheral & CLKPWR_PCONP_BITMASK;
    } else {
        LPC_SC->PCONP &= (~Peripheral) & CLKPWR_PCONP_BITMASK;
    }
}

void SetPeripheralClockDivider (
    CLKPWR_PCLKSEL Peripheral,
    CLKPWR_PCLKSEL_CCLK_DIV Divider
    )
{
    if (Peripheral < 32) {
        LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(0x3 << Peripheral)) | 
            (Divider << Peripheral);
    } else {
        LPC_SC->PCLKSEL1 = (LPC_SC->PCLKSEL1 & ~(0x3 << (Peripheral - 32))) | 
            (Divider << (Peripheral - 32));
    }
}

CLKPWR_PCLKSEL_CCLK_DIV GetPeripheralClockDivider (CLKPWR_PCLKSEL Peripheral)
{
    uint32_t value;
    
	if (Peripheral < 32) {
        value = (LPC_SC->PCLKSEL0 >> Peripheral) & 0x3;
	} else {
        value = (LPC_SC->PCLKSEL1 >> (Peripheral - 32)) & 0x3;
	}
    
    return CLKPWR_PCLKSEL_CCLK_DIV(value);
}

uint32_t DividerFromCclkDiv (CLKPWR_PCLKSEL_CCLK_DIV Value)
{
    switch (Value) {
    case CLKPWR_PCLKSEL_CCLK_DIV_4: return 4;
    case CLKPWR_PCLKSEL_CCLK_DIV_1: return 1;
    case CLKPWR_PCLKSEL_CCLK_DIV_2: return 2;
    case CLKPWR_PCLKSEL_CCLK_DIV_8: return 8;
    default: return 0;
    }
}

uint32_t GetPeripheralClockFrequency (CLKPWR_PCLKSEL Peripheral)
{
    uint32_t divider = 
        DividerFromCclkDiv(GetPeripheralClockDivider(Peripheral));
    
    return SystemCoreClock / divider;
}
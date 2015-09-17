//
// Copyright (C) Microsoft. All rights reserved.
//
#include <stdint.h>
#include <lpc17xx.h>

#include "Lpc17xxHardware.h"

LPC_TIM_TypeDef* _defaultTimer;

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

void SetDefaultTimer (LPC_TIM_TypeDef* Timer)
{
    _defaultTimer = Timer;

    CLKPWR_PCONP pconp;
    CLKPWR_PCLKSEL pclksel;
    if (Timer == LPC_TIM0) {
        pconp = CLKPWR_PCONP_PCTIM0;
        pclksel = CLKPWR_PCLKSEL_TIMER0;
    } else if (Timer == LPC_TIM1) {
        pconp = CLKPWR_PCONP_PCTIM1;
        pclksel = CLKPWR_PCLKSEL_TIMER1;
    } else if (Timer == LPC_TIM2) {
        pconp = CLKPWR_PCONP_PCTIM2;
        pclksel = CLKPWR_PCLKSEL_TIMER2;
    } else {
        pconp = CLKPWR_PCONP_PCTIM3;
        pclksel = CLKPWR_PCLKSEL_TIMER3;
    }

    SetPeripheralPowerState(pconp, true);
    SetPeripheralClockDivider(pclksel, CLKPWR_PCLKSEL_CCLK_DIV_4);

    Timer->TCR = TIM_TCR_RESET;
    Timer->CTCR = 0;            // timer mode
    Timer->PR = SystemCoreClock / (4 * 1000000) - 1;        // 1us resolution
    Timer->MCR = 0;
    Timer->CCR = 0;
    Timer->IR = 0x3f;           // clear interrupts
    // take out of reset and enable for counting
    Timer->TCR = TIM_TCR_ENABLE;
}

uint32_t Micros ()
{
    return _defaultTimer->TC;
}

void DelayMicros (uint32_t Micros)
{
    uint32_t start = _defaultTimer->TC;
    uint32_t end = start + Micros;

    if(end < start) {       // overflow condition
        while(_defaultTimer->TC >= start);
        while(_defaultTimer->TC < end);
    }  else {
        while(_defaultTimer->TC < end && _defaultTimer->TC >= start);
    }
}

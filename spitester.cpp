//
// Copyright (C) Microsoft. All rights reserved.
//
#include <stdio.h>
#include <algorithm>
#include <lpc17xx.h>
#include "Lpc17xxHardware.h"
#include "util.h"
#include "spitester.h"

using namespace Lldt::Spi;

uint32_t SpiTester::dummy;

void SpiTester::Init ()
{
    SspInit();
    TimerInit();
    
    uint32_t sspClk = GetPeripheralClockFrequency(CLKPWR_PCLKSEL_SSP0);
    
    this->testerInfo.DeviceId = DEVICE_ID;
    this->testerInfo.Version = VERSION;
    this->testerInfo.MaxFrequency = std::min(uint32_t(5000000), sspClk / 12);
    this->testerInfo.ClockMeasurementFrequency = SystemCoreClock;
    this->testerInfo.MinDataBitLength = MIN_DATA_BIT_LENGTH;
    this->testerInfo.MaxDataBitLength = MAX_DATA_BIT_LENGTH;
    
    printf(
        "sspClk = %lu, Maximum clock rate = %lu\n\r",
        sspClk,
        this->testerInfo.MaxFrequency);
}

//
// Initialize SSP0 in slave mode
//
void SpiTester::SspInit ()
{
    // Power
    SetPeripheralPowerState(CLKPWR_PCONP_PCSSP0, true);

    // Clock (set to maximum)
    SetPeripheralClockDivider(CLKPWR_PCLKSEL_SSP0, CLKPWR_PCLKSEL_CCLK_DIV_1);

    // Configure Pins

    // SCK0 (P0.15)
    LPC_PINCON->PINSEL0 = (LPC_PINCON->PINSEL0 & ~(0x3 << 30)) | (0x2 << 30);

    // SSEL0 (P0.16), MISO0 (P0.17), MOSI (P0.18)
    uint32_t temp = (LPC_PINCON->PINSEL1 & ~((0x3 << 2) | (0x3 << 4) | (0x3 << 0)));
    temp |= (0x2 << 2) | (0x2 << 4) | (0x2 << 0);
    LPC_PINCON->PINSEL1 = temp;

    // Disable interrupts
    LPC_SSP0->IMSC = 0;
    LPC_SSP0->CPSR = 2;

    // Program control registers and enable
    SspSetDataMode(
        SPI_CONTROL_INTERFACE_MODE,
        SPI_CONTROL_INTERFACE_DATABITLENGTH);
}

void SpiTester::SspSetDataMode (SpiDataMode Mode, uint32_t DataBitLength)
{
    uint32_t cr0 = SSP_CR0_FRF_SPI;

    switch (Mode) {
    case SpiDataMode::Mode1:
    case SpiDataMode::Mode2:
        cr0 |= SSP_CR0_CPHA_SECOND;
        break;
    case SpiDataMode::Mode0:
    case SpiDataMode::Mode3:
    default:
        cr0 |= SSP_CR0_CPOL_HI | SSP_CR0_CPHA_SECOND;
        break;
    }

    if ((DataBitLength >= MIN_DATA_BIT_LENGTH) &&
        (DataBitLength <= MAX_DATA_BIT_LENGTH)) {

        cr0 |= SSP_CR0_DSS(DataBitLength);
    } else {
        cr0 |= SSP_CR0_DSS(8);
    }

    LPC_SSP0->CR1 = SSP_CR1_SLAVE_EN;
    LPC_SSP0->CR0 = cr0;
    LPC_SSP0->CR1 = SSP_CR1_SSP_EN | SSP_CR1_SLAVE_EN;
}

void SpiTester::SspSendWithChecksum (const uint8_t* Data, uint32_t LengthInBytes)
{
    __disable_irq();
    
    for (const uint8_t* data = Data; data != (Data + LengthInBytes); ) {
        if (LPC_SSP0->SR & SSP_SR_TNF) {
            LPC_SSP0->DR = *data;
            ++data;
        }
    }
    
    uint32_t checksum = Crc16().Update(Data, LengthInBytes);
    for (size_t i = 0; i < sizeof(uint16_t); ) {
        if (LPC_SSP0->SR & SSP_SR_TNF) {
            LPC_SSP0->DR = reinterpret_cast<uint8_t*>(&checksum)[i];
            ++i;
        }
    }
    
    __enable_irq();
    
    WaitForCsToDeassert();
}

void SpiTester::WaitForCsToDeassert ()
{    
    while (ChipSelectAsserted() || (LPC_SSP0->SR & SSP_SR_RNE))
        dummy = LPC_SSP0->DR;
}

//
// Initialize TIMER2 to capture inputs on CAP2.0
//
void SpiTester::TimerInit ()
{
    // Initialize clock and power, use highest posible resolution
    SetPeripheralPowerState(CLKPWR_PCONP_PCTIM2, true);
    SetPeripheralClockDivider(CLKPWR_PCLKSEL_TIMER2, CLKPWR_PCLKSEL_CCLK_DIV_1);

    // P0.4 - CAP2.0 - I - Capture input for Timer 2, channel 0.
    LPC_PINCON->PINSEL0 |= 0x3 << 8;

    // Put timer in reset
    LPC_TIM2->TCR = TIM_TCR_RESET;

    // Timer mode
    LPC_TIM2->TCR = 0;

    // Incrent Timer Counter on every PCLK
    LPC_TIM2->PR = 0;

    // Stop the counter if overflow is detected
    LPC_TIM2->MCR = TIM_MCR_STOP_ON_MATCH(TIM_MATCH_CHANNEL_0);
    LPC_TIM2->MR0 = 0xffffffff;

    // Capture CR0 on falling edge
    LPC_TIM2->CCR = TIM_CCR_FALLING(TIM_CAPTURE_CHANNEL_0);
}

ClockMeasurementStatus SpiTester::WaitForCapture (uint32_t* Capture)
{
    // wait for first capture or first byte to be received
    uint32_t capture;
    
    while (!(LPC_SSP0->SR & SSP_SR_RNE)) {
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
        if ((capture = LPC_TIM2->CR0) != 0) break;
    }
    
    if (capture != 0) {
        *Capture = capture;
        return ClockMeasurementStatus::Success;
    }
    
    return ClockMeasurementStatus::EdgeNotDetected;
}

TransferInfo SpiTester::CaptureTransfer (const CommandBlock& Command)
{
    // printf(
        // "Validating data with Mode = %d, DataBitLength = %d\n\r",
        // Command.u.CaptureNextTransfer.Mode,
        // Command.u.CaptureNextTransfer.DataBitLength);

    auto transferInfo = TransferInfo();
        
    Crc16 checksum;
    const uint32_t dataMask = (1 << Command.u.CaptureNextTransfer.DataBitLength) - 1;
    // This is the value we should expect to receive from the master
    uint32_t rxValue = Command.u.CaptureNextTransfer.SendValue;
    // This is the value we should send to the master
    uint32_t txValue = Command.u.CaptureNextTransfer.ReceiveValue;
    bool mismatchDetected = false;

    SspSetDataMode(
        SpiDataMode(Command.u.CaptureNextTransfer.Mode),
        Command.u.CaptureNextTransfer.DataBitLength);

    // Reset timer
    LPC_TIM2->TCR = TIM_TCR_RESET;
        
    __disable_irq();
    
    // do initial fill of TX fifo
    for (int i = 0; i < 8; ++i) {
        LPC_SSP0->DR = txValue & dataMask;
        ++txValue;
    }
    
    // Wait for CS to assert
    while (!ChipSelectAsserted());
    
    // start timer
    LPC_TIM2->TCR = TIM_TCR_ENABLE;
    
    uint32_t capture1;
    transferInfo.ClockActiveTimeStatus = WaitForCapture(&capture1);

    for (;;) {
        // byte received?
        uint32_t status = LPC_SSP0->SR;

        // space available in TX FIFO?
        if (status & SSP_SR_TNF) {
            LPC_SSP0->DR = txValue & dataMask;
            ++txValue;
        }
        
        if (status & SSP_SR_RNE) {
            uint32_t data = LPC_SSP0->DR;
            
            // add to checksum
            checksum.Update(uint8_t(data));
            if (Command.u.CaptureNextTransfer.DataBitLength > 8) {
                checksum.Update(uint8_t(data >> 8));
            }

            if ((data != (rxValue & dataMask)) && !mismatchDetected) {
                mismatchDetected = true;
                transferInfo.MismatchIndex = 
                    rxValue - Command.u.CaptureNextTransfer.SendValue;
                // printf(
                    // "Mismatch detected! (rxValue = %d, data = 0x%x)\n\r",
                    // rxValue,
                    // data);
            }
            ++rxValue;
        } else if (!ChipSelectAsserted()) {
            // only check if chip select is deasserted if the receive FIFO
            // has been purged
            break;
        }
    }
    
    __enable_irq();
    
    if (transferInfo.ClockActiveTimeStatus == ClockMeasurementStatus::Success) {
        // did timer overflow?
        if (!(LPC_TIM2->TCR & TIM_TCR_ENABLE)) {
            transferInfo.ClockActiveTimeStatus = ClockMeasurementStatus::Overflow;
        } else {
            // measurement was captured successfully        
            uint32_t capture2 = LPC_TIM2->CR0;
            LPC_TIM2->TCR = TIM_TCR_RESET;
            
            transferInfo.ClockActiveTime = capture2 - capture1;
            // printf(
                // "Time measured successfully. capture = %lu, capture2 = %lu, activeTime = %lu, clockActiveTime = %lu\n\r",
                // capture,
                // capture2,
                // activeTime,
                // clockActiveTime);
        }
    }
    
    // printf("data = 0x%x\n\r", data);
    
    transferInfo.Checksum = checksum.Get();
    transferInfo.ElementCount = rxValue - Command.u.CaptureNextTransfer.SendValue;
    if (!mismatchDetected)
        transferInfo.MismatchIndex = transferInfo.ElementCount;
    
    // printf(
        // "transferInfo.ElementCount = %d, rxValue = %d\n\r",
        // transferInfo.ElementCount,
        // rxValue);
    SspSetDataMode(
        SPI_CONTROL_INTERFACE_MODE,
        SPI_CONTROL_INTERFACE_DATABITLENGTH);
        
    return transferInfo;
}


bool SpiTester::ReceiveCommand (CommandBlock& Command)
{
    // is there any data waiting for us?
    if (!(LPC_SSP0->SR & SSP_SR_RNE)) return false;
    
    // receive a command block
    for (size_t i = 0; i < sizeof(Command); ) {
        // byte received?
        if (LPC_SSP0->SR & SSP_SR_RNE) {
            uint32_t data = LPC_SSP0->DR;
            reinterpret_cast<uint8_t*>(&Command)[i] = uint8_t(data);
            ++i;
        } else if (!ChipSelectAsserted()) {
            return false;
        }
    }

    WaitForCsToDeassert();
    return true;
}

void SpiTester::RunStateMachine ()
{
    CommandBlock command;
    if (ReceiveCommand(command)) {
        switch (command.Command) {
        case SpiTesterCommand::GetDeviceInfo:
            SspSendWithChecksum(this->testerInfo);
            break;
        case SpiTesterCommand::CaptureNextTransfer:
            this->transferInfo = CaptureTransfer(command);
            break;
        case SpiTesterCommand::GetTransferInfo:
            SspSendWithChecksum(this->transferInfo);
            break;
        default:
            // printf("Invalid command: 0x%x\n\r", command);
            break;
        }
    }    
}
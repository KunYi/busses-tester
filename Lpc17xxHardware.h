//
// Copyright (C) Microsoft. All rights reserved.
//
#ifndef _LPC17XX_HARDWARE_H_
#define _LPC17XX_HARDWARE_H_

//
// Clock and Power
//

enum CLKPWR_PCLKSEL : uint32_t {
    CLKPWR_PCLKSEL_WDT      = 0U,
    CLKPWR_PCLKSEL_TIMER0   = 2U,
    CLKPWR_PCLKSEL_TIMER1   = 4U,
    CLKPWR_PCLKSEL_UART0    = 6U,
    CLKPWR_PCLKSEL_UART1    = 8U,
    CLKPWR_PCLKSEL_PWM1     = 12U,
    CLKPWR_PCLKSEL_I2C0     = 14U,
    CLKPWR_PCLKSEL_SPI      = 16U,
    CLKPWR_PCLKSEL_SSP1     = 20U,
    CLKPWR_PCLKSEL_DAC      = 22U,
    CLKPWR_PCLKSEL_ADC      = 24U,
    CLKPWR_PCLKSEL_CAN1     = 26U,
    CLKPWR_PCLKSEL_CAN2     = 28U,
    CLKPWR_PCLKSEL_ACF      = 30U,
    CLKPWR_PCLKSEL_QEI      = 32U,
    CLKPWR_PCLKSEL_PCB      = 36U,
    CLKPWR_PCLKSEL_I2C1     = 38U,
    CLKPWR_PCLKSEL_SSP0     = 42U,
    CLKPWR_PCLKSEL_TIMER2   = 44U,
    CLKPWR_PCLKSEL_TIMER3   = 46U,
    CLKPWR_PCLKSEL_UART2    = 48U,
    CLKPWR_PCLKSEL_UART3    = 50U,
    CLKPWR_PCLKSEL_I2C2     = 52U,
    CLKPWR_PCLKSEL_I2S      = 54U,
    CLKPWR_PCLKSEL_RIT      = 58U,
    CLKPWR_PCLKSEL_SYSCON   = 60U,
    CLKPWR_PCLKSEL_MC       = 62U,
};

enum CLKPWR_PCLKSEL_CCLK_DIV : uint32_t {
    CLKPWR_PCLKSEL_CCLK_DIV_4 = 0U,
    CLKPWR_PCLKSEL_CCLK_DIV_1 = 1U,
    CLKPWR_PCLKSEL_CCLK_DIV_2 = 2U,
    CLKPWR_PCLKSEL_CCLK_DIV_8 = 3U,
};


enum CLKPWR_PCONP : uint32_t {
    CLKPWR_PCONP_PCTIM0     = 1U<<1,
    CLKPWR_PCONP_PCTIM1     = 1U<<2,
    CLKPWR_PCONP_PCUART0    = 1U<<3,
    CLKPWR_PCONP_PCUART1    = 1U<<4,
    CLKPWR_PCONP_PCPWM1     = 1U<<6,
    CLKPWR_PCONP_PCI2C0     = 1U<<7,
    CLKPWR_PCONP_PCSPI      = 1U<<8,
    CLKPWR_PCONP_PCRTC      = 1U<<9,
    CLKPWR_PCONP_PCSSP1     = 1U<<10,
    CLKPWR_PCONP_PCAD       = 1U<<12,
    CLKPWR_PCONP_PCAN1      = 1U<<13,
    CLKPWR_PCONP_PCAN2      = 1U<<14,
    CLKPWR_PCONP_PCGPIO     = 1U<<15,
    CLKPWR_PCONP_PCRIT      = 1U<<16,
    CLKPWR_PCONP_PCMC       = 1U<<17,
    CLKPWR_PCONP_PCQEI      = 1U<<18,
    CLKPWR_PCONP_PCI2C1     = 1U<<19,
    CLKPWR_PCONP_PCSSP0     = 1U<<21,
    CLKPWR_PCONP_PCTIM2     = 1U<<22,
    CLKPWR_PCONP_PCTIM3     = 1U<<23,
    CLKPWR_PCONP_PCUART2    = 1U<<24,
    CLKPWR_PCONP_PCUART3    = 1U<<25,
    CLKPWR_PCONP_PCI2C2     = 1U<<26,
    CLKPWR_PCONP_PCI2S      = 1U<<27,
    CLKPWR_PCONP_PCGPDMA    = 1U<<29,
    CLKPWR_PCONP_PCENET     = 1U<<30,
    CLKPWR_PCONP_PCUSB      = 1U<<31,
    CLKPWR_PCONP_BITMASK    = 0xEFEFF7DEU,
};

enum CLKPWR_CLKSRCSEL : uint32_t {
    CLKPWR_CLKSRCSEL_CLKSRC_IRC          = 0x00U,
    CLKPWR_CLKSRCSEL_CLKSRC_MAINOSC      = 0x01U,
    CLKPWR_CLKSRCSEL_CLKSRC_RTC          = 0x02U,
    CLKPWR_CLKSRCSEL_BITMASK             = 0x03U,
};

enum CLKPWR_CLKOUTCFG : uint32_t {
    CLKPWR_CLKOUTCFG_CLKOUTSEL_CPU       = 0x00U,
    CLKPWR_CLKOUTCFG_CLKOUTSEL_MAINOSC   = 0x01U,
    CLKPWR_CLKOUTCFG_CLKOUTSEL_RC        = 0x02U,
    CLKPWR_CLKOUTCFG_CLKOUTSEL_USB       = 0x03U,
    CLKPWR_CLKOUTCFG_CLKOUTSEL_RTC       = 0x04U,
    CLKPWR_CLKOUTCFG_CLKOUT_EN           = 1U<<8,
    CLKPWR_CLKOUTCFG_CLKOUT_ACT          = 1U<<9,
    CLKPWR_CLKOUTCFG_BITMASK             = 0x3FFU,
};

enum CLKPWR_PLL0CON : uint32_t {
    CLKPWR_PLL0CON_ENABLE       = 0x01U,
    CLKPWR_PLL0CON_CONNECT      = 0x02U,
    CLKPWR_PLL0CON_BITMASK      = 0x03U,
};

enum CLKPWR_PCON : uint32_t {
    CLKPWR_PCON_PM0         = 1U<<0,
    CLKPWR_PCON_PM1         = 1U<<1,
    CLKPWR_PCON_BODPDM      = 1U<<2,
    CLKPWR_PCON_BOGD        = 1U<<3,
    CLKPWR_PCON_BORD        = 1U<<4,
    CLKPWR_PCON_SMFLAG      = 1U<<8,
    CLKPWR_PCON_DSFLAG      = 1U<<9,
    CLKPWR_PCON_PDFLAG      = 1U<<10,
    CLKPWR_PCON_DPDFLAG     = 1U<<11,
};

void SetPeripheralPowerState (CLKPWR_PCONP Peripheral, bool Enable);
void SetPeripheralClockDivider (
    CLKPWR_PCLKSEL Peripheral,
    CLKPWR_PCLKSEL_CCLK_DIV Divider
    );

CLKPWR_PCLKSEL_CCLK_DIV GetPeripheralClockDivider (CLKPWR_PCLKSEL Peripheral);
uint32_t DividerFromCclkDiv (CLKPWR_PCLKSEL_CCLK_DIV Value);
uint32_t GetPeripheralClockFrequency (CLKPWR_PCLKSEL Peripheral);

//
// Timer Related Definitions
//
enum TIM_TCR : uint32_t {
    TIM_TCR_ENABLE          = 1<<0,
    TIM_TCR_RESET           = 1<<1,
    TIM_TCR_BITMASK         = 3,
};

//
// Timer MCR Register Bit Definitions
//
enum TIM_MATCH_CHANNEL {
    TIM_MATCH_CHANNEL_0,
    TIM_MATCH_CHANNEL_1,
    TIM_MATCH_CHANNEL_2,
    TIM_MATCH_CHANNEL_3,
};

constexpr inline uint32_t TIM_MCR_INT_ON_MATCH (TIM_MATCH_CHANNEL Channel)
{
    return 1U << (Channel * 3);
}

constexpr inline uint32_t TIM_MCR_RESET_ON_MATCH (TIM_MATCH_CHANNEL Channel)
{
    return 1U << ((Channel * 3) + 1);
}

constexpr inline uint32_t TIM_MCR_STOP_ON_MATCH (TIM_MATCH_CHANNEL Channel)
{
    return 1U << ((Channel * 3) + 2);
}

//
// Timer CCR Register Bit Definitions
//

enum TIM_CAPTURE_CHANNEL {
    TIM_CAPTURE_CHANNEL_0,
    TIM_CAPTURE_CHANNEL_1,
    TIM_CAPTURE_CHANNEL_2,
    TIM_CAPTURE_CHANNEL_3,
};

constexpr inline uint32_t TIM_CCR_RISING (TIM_CAPTURE_CHANNEL Channel)
{
    return 1U << (Channel * 3);
}

constexpr inline uint32_t TIM_CCR_FALLING (TIM_CAPTURE_CHANNEL Channel)
{
    return 1U << ((Channel * 3) + 1);
}

constexpr inline uint32_t TIM_CCR_INT_ON_CAP (TIM_CAPTURE_CHANNEL Channel)
{
    return 1U << ((Channel * 3) + 2);
}

//
// Timer CTCR Register Bit Definitions
//
enum TIM_CTCR : uint32_t {
    TIM_CTCR_MODE_MASK      = 0x3,
    TIM_CTCR_INPUT_MASK     = 0xC,
    TIM_CTCR_MASKBIT        = 0xF,
    TIM_CTCR_COUNTER_MODE   = 1,
};

void SetDefaultTimer (LPC_TIM_TypeDef* Timer);
uint32_t Micros ();
void DelayMicros (uint32_t Micros);

inline uint32_t Millis ()
{
    return Micros() / 1000;
}

inline void DelayMillis (uint32_t Millis)
{
    DelayMicros(1000U * Millis);
}


//
// I2C Related Definitions
//

//
// I2CONSET register field definitions
//
enum I2C_I2CONSET : uint32_t {
    I2C_I2CONSET_AA             = 1 << 2,
    I2C_I2CONSET_SI             = 1 << 3,
    I2C_I2CONSET_STO            = 1 << 4,
    I2C_I2CONSET_STA            = 1 << 5,
    I2C_I2CONSET_I2EN           = 1 << 6,
};

//
// I2CONCLR register field definitions
//
enum I2C_I2CONCLR : uint32_t {
    I2C_I2CONCLR_AAC            = 1 << 2,
    I2C_I2CONCLR_SIC            = 1 << 3,
    I2C_I2CONCLR_STOC           = 1 << 4,
    I2C_I2CONCLR_STAC           = 1 << 5,
    I2C_I2CONCLR_I2ENC          = 1 << 6,
};

//
// I2STAT register status code definitions
//
enum I2C_STAT : uint32_t {
    I2C_I2STAT_NO_INF                       = 0xF8,
    I2C_I2STAT_BUS_ERROR                    = 0x00,

    // Master transmitter states
    I2C_I2STAT_M_TX_START                   = 0x08,
    I2C_I2STAT_M_TX_RESTART                 = 0x10,
    I2C_I2STAT_M_TX_SLAW_ACK                = 0x18,
    I2C_I2STAT_M_TX_SLAW_NACK               = 0x20,
    I2C_I2STAT_M_TX_DAT_ACK                 = 0x28,
    I2C_I2STAT_M_TX_DAT_NACK                = 0x30,
    I2C_I2STAT_M_TX_ARB_LOST                = 0x38,

    // Master receiver states
    I2C_I2STAT_M_RX_START                   = 0x08,
    I2C_I2STAT_M_RX_RESTART                 = 0x10,
    I2C_I2STAT_M_RX_ARB_LOST                = 0x38,
    I2C_I2STAT_M_RX_SLAR_ACK                = 0x40,
    I2C_I2STAT_M_RX_SLAR_NACK               = 0x48,
    I2C_I2STAT_M_RX_DAT_ACK                 = 0x50,
    I2C_I2STAT_M_RX_DAT_NACK                = 0x58,

    // Slave receiver states
    I2C_I2STAT_S_RX_SLAW_ACK                = 0x60,
    I2C_I2STAT_S_RX_ARB_LOST_M_SLA          = 0x68,
    I2C_I2STAT_S_RX_GENCALL_ACK             = 0x70,
    I2C_I2STAT_S_RX_ARB_LOST_M_GENCALL      = 0x78,
    I2C_I2STAT_S_RX_PRE_SLA_DAT_ACK         = 0x80,
    I2C_I2STAT_S_RX_PRE_SLA_DAT_NACK        = 0x88,
    I2C_I2STAT_S_RX_PRE_GENCALL_DAT_ACK     = 0x90,
    I2C_I2STAT_S_RX_PRE_GENCALL_DAT_NACK    = 0x98,
    I2C_I2STAT_S_RX_STA_STO_SLVREC_SLVTRX   = 0xA0,

    // Slave trasmitter states
    I2C_I2STAT_S_TX_SLAR_ACK                = 0xA8,
    I2C_I2STAT_S_TX_ARB_LOST_M_SLA          = 0xB0,
    I2C_I2STAT_S_TX_DAT_ACK                 = 0xB8,
    I2C_I2STAT_S_TX_DAT_NACK                = 0xC0,
    I2C_I2STAT_S_TX_LAST_DAT_ACK            = 0xC8,
};

//
// SSP Related Definitions
//

//
// SSP CR0 Register Bit Definitions
//
enum SSP_CR0 : uint32_t {
    SSP_CR0_FRF_SPI         = 0<<4,
    SSP_CR0_FRF_TI          = 1<<4,
    SSP_CR0_FRF_MICROWIRE   = 2<<4,
    SSP_CR0_CPOL_HI         = 1<<6,
    SSP_CR0_CPHA_SECOND     = 1<<7,
    SSP_CR0_BITMASK         = 0xFFFF,
};

//
// SSP CR1 Register Bit Definitions
//
enum SSP_CR1 : uint32_t {
    SSP_CR1_LBM_EN       = 1<<0,
    SSP_CR1_SSP_EN       = 1<<1,
    SSP_CR1_SLAVE_EN     = 1<<2,
    SSP_CR1_SO_DISABLE   = 1<<3,
    SSP_CR1_BITMASK      = 0x0F,
};

//
// SSP SR Register Bit Definitions
//
enum SSP_SR : uint32_t {
    SSP_SR_TFE       = 1<<0,
    SSP_SR_TNF       = 1<<1,
    SSP_SR_RNE       = 1<<2,
    SSP_SR_RFF       = 1<<3,
    SSP_SR_BSY       = 1<<4,
    SSP_SR_BITMASK   = 0x1F,
};

//
// SSP IMSC Register Bit Definitions
//
enum SSP_IMSC : uint32_t {
    SSP_IMSC_ROR     = 1<<0,
    SSP_IMSC_RT      = 1<<1,
    SSP_IMSC_RX      = 1<<2,
    SSP_IMSC_TX      = 1<<3,
    SSP_IMSC_BITMASK = 0x0F,
};

//
// SSP RIS Register Bit Definitions
//
enum SSP_RIS : uint32_t {
    SSP_RIS_ROR      = 1<<0,
    SSP_RIS_RT       = 1<<1,
    SSP_RIS_RX       = 1<<2,
    SSP_RIS_TX       = 1<<3,
    SSP_RIS_BITMASK  = 0x0F,
};

//
// SSP MIS Register Bit Definitions
//
enum SSP_MIS : uint32_t {
    SSP_MIS_ROR      = 1<<0,
    SSP_MIS_RT       = 1<<1,
    SSP_MIS_RX       = 1<<2,
    SSP_MIS_TX       = 1<<3,
    SSP_MIS_BITMASK  = 0x0F,
};

//
// SSP ICR Register Bit Definitions
//
enum SSP_ICR : uint32_t {
    SSP_ICR_ROR      = 1<<0,
    SSP_ICR_RT       = 1<<1,
    SSP_ICR_BITMASK  = 0x03,
};

//
// SSP DMACR Register Bit Definitions
//
enum SSP_DMACR : uint32_t {
    SSP_DMACR_RXDMA_EN       = 1<<0,
    SSP_DMACR_TXDMA_EN       = 1<<1,
    SSP_DMACR_BITMASK        = 0x03,
};

constexpr inline uint32_t SSP_CR0_DSS (uint32_t DataBitLength)
{
    return (DataBitLength - 1) & 0xF;
}

constexpr inline uint32_t SSP_CR0_SCR (uint32_t Value)
{
    return (Value & 0xFF) << 8;
}

#endif // _LPC17XX_HARDWARE_H_
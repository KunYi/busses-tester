/*
    Copyright(c) Microsoft Open Technologies, Inc. All rights reserved.

    The MIT License(MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files(the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions :

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#ifndef _I2CDEF_H_
#define _I2CDEF_H_

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

#endif // _I2CDEF_H_
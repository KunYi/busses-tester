//
// Copyright (C) Microsoft. All rights reserved.
//
#ifndef _LLDTESTER_UTIL_H_
#define _LLDTESTER_UTIL_H_

#include <utility>

#ifdef _DEBUG
#define DBGPRINT printf
#else
#define DBGPRINT(...)
#endif // _DEBUG

class Crc16
{
public:

    Crc16 ( ) : crc(0) { }

    uint32_t Get ( ) const { return this->crc; }

    uint32_t Update ( uint8_t data )
    {
        return (this->crc = ((this->crc << 8) ^
            crc16table[((this->crc >> 8) ^ data) & 0xff]) & 0xffff);
    }

    uint32_t Update (const uint8_t* Data, unsigned int Length)
    {
        unsigned int tempCrc = this->crc;
        for (const uint8_t *data = Data; data != (Data + Length); ++data)
            tempCrc = (tempCrc << 8) ^ crc16table[((tempCrc >> 8) ^ *data) & 0xff];
        return (this->crc = (tempCrc & 0xffff));
    }

    void Reset ( ) { this->crc = 0; }

    static const uint16_t crc16table[];

private:

    uint32_t crc;
};

inline uint32_t crc16_update (uint32_t crc, uint8_t data)
{
    return ((crc << 8) ^ Crc16::crc16table[((crc >> 8) ^ data) & 0xff]) & 0xffff;
}

struct DisableIrq {
    DisableIrq () { __disable_irq(); }
    ~DisableIrq () { __enable_irq(); }
};

template <typename Fn>
struct _Finally : public Fn {
    _Finally (Fn&& Func) : Fn(Func) {}
    ~_Finally () { (*this)(); }
};

template <typename Fn>
inline _Finally<Fn> Finally (Fn&& Func)
{
    return std::forward<Fn>(Func);
}

// P1.18
inline void ActLedInit ()
{
    LPC_GPIO1->FIODIR |= 1 << 18;
}

inline void ActLedOn ()
{
    LPC_GPIO1->FIOSET = 1 << 18;
}

inline void ActLedOff ()
{
    LPC_GPIO1->FIOCLR = 1 << 18;
}

// P1.20
inline void ErrLedInit ()
{
    LPC_GPIO1->FIODIR |= 1 << 20;
}

inline void ErrLedOn ()
{
    LPC_GPIO1->FIOSET = 1 << 20;
}

inline void ErrLedOff ()
{
    LPC_GPIO1->FIOCLR = 1 << 20;
}

#endif // _LLDTESTER_UTIL_H_

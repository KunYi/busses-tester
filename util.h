#ifndef _LLDTESTER_UTIL_H_
#define _LLDTESTER_UTIL_H_

#include <stdint.h>

class Crc16
{
public:

    Crc16 ( ) : crc(0) { }

    uint32_t Get ( ) const { return this->crc & 0xffff; }

    uint32_t Update ( uint8_t data )
    {
        this->crc = (this->crc << 8) ^
            crc16table[((this->crc >> 8) ^ data) & 0xff];
        return this->Get();
    }
    
    uint32_t Update (const uint8_t* Data, unsigned int Length)
    {
        unsigned int tempCrc = 0;
        for (const uint8_t *data = Data; data != (Data + Length); ++data)
            tempCrc = (tempCrc << 8) ^ crc16table[((tempCrc >> 8) ^ *data) & 0xff];
        this->crc = tempCrc;
        return tempCrc & 0xffff;
    }

    void Reset ( ) { this->crc = 0; }

private:
    static const uint16_t crc16table[];

    uint32_t crc;
};

#endif // _LLDTESTER_UTIL_H_

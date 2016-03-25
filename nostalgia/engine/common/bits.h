#ifndef _GY_BITS_H_
#define _GY_BITS_H_

// Operations with bitmasks
class gyBits
{
public:
  static inline void PackByte( uint32_t& number, uint32_t bytePos, uint8_t val );
  static inline uint8_t UnpackByte( uint32_t number, uint32_t bytepos);
};


// Implementation
void gyBits::PackByte( uint32_t& number, uint32_t bytePos, uint8_t val )
{ 
  const uint32_t bits=bytePos<<3; 
  number = (number&~(0xff<<bits)) | (val<<bits); 
}

uint8_t gyBits::UnpackByte( uint32_t number, uint32_t bytepos) 
{ 
  const uint32_t bits=bytepos<<3; return (uint32_t) (number&(0xff<<bits))>>bits; 
}


#endif
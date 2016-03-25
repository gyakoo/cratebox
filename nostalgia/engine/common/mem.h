#ifndef _GY_MEM_H_
#define _GY_MEM_H_

class gyMem
{
public:
  inline static void FillMem( void* data, uint32_t sizeInBytes, uint8_t val );
  inline static void FillMemT( float* values, uint32_t count, float val );
};

#include <engine/common/mem.inl>

#endif
void gyMem::FillMem( void* data, uint32_t sizeInBytes, uint8_t val )
{
#ifdef BX_PLATFORM_WINDOWS
  ::memset(data, val, sizeInBytes);
#endif
}

void gyMem::FillMemT( float* values, uint32_t count, float val )
{
  for ( register int i = count-1; i >= 0; --i ) 
    values[i]=val;
}
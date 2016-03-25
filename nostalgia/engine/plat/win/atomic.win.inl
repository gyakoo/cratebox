#if defined(_WIN32) && defined(BX_PLATFORM_WINDOWS) // only supported on 64bit windows
#include <intrin.h>
#endif

int gyAtomicIncrement(int32_t &i32)
{
  return InterlockedIncrement((LONG*) &i32);
}

int gyAtomicDecrement(int32_t &i32)
{
  return InterlockedDecrement((LONG*) &i32);
}

void gyAtomicAdd(int32_t& dest, int32_t value)
{
  InterlockedExchangeAdd((LONG*)&dest, value);
}

void gyAtomicAdd(int64_t& dest, int64_t value)
{
#if defined(_WIN64) // only supported on 64bit windows
  InterlockedExchangeAdd64((LONG64*)&dest, value);
#else
  while(true)
  {
    int64_t oldVal = dest;
    int64_t newVal = dest + value;

    if (_InterlockedCompareExchange64(&dest, newVal, oldVal) == oldVal)
      break;
  }
#endif
}

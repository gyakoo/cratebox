/// A high resolution timer, to measure time between start and stop calls
gyClock::gyClock()
  : invFreq(0), mStart(0), mSplit(0)
{
  int64_t freq;
  QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
  invFreq = 1.0/freq;
}
  
void gyClock::Start( )
{
  DWORD_PTR oldmask = ::SetThreadAffinityMask(::GetCurrentThread(), 0);
  QueryPerformanceCounter( (LARGE_INTEGER*)&mStart );
  mSplit = mStart;
  ::SetThreadAffinityMask(::GetCurrentThread(), oldmask);
}

double gyClock::Stop( )
{
  int64_t t2;
  QueryPerformanceCounter( (LARGE_INTEGER*)&t2 );
  double ret = double( (t2-mStart)*invFreq );
  mStart = mSplit = t2;
  return ret;
}

double gyClock::Split()
{
  int64_t t2;
  QueryPerformanceCounter( (LARGE_INTEGER*)&t2 );
  const double ret = double( (t2-mSplit)*invFreq ); 
  mSplit = t2;
  return ret;
}

double gyClock::TimeFromStart()
{
  int64_t t2;
  QueryPerformanceCounter( (LARGE_INTEGER*)&t2 );
  return double( (t2-mStart)*invFreq );
}
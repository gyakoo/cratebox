#ifndef _CLOCK_H_
#define _CLOCK_H_

/// A high resolution timer, to measure time between start and stop calls
class gyClock
{
public:
  gyClock();
  void Start( );
  double Stop( );
  double Split();
  double TimeFromStart();

protected:
#ifdef BX_PLATFORM_WINDOWS
  double invFreq;
  int64_t mStart;
  int64_t mSplit;
#endif
};

#endif
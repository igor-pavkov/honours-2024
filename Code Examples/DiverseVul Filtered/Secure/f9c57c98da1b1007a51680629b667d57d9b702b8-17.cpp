uint64_t countLessThan(unsigned int msec)
{
  uint64_t ret=0;
  for(unsigned int i = 0 ; i < msec && i < flightTimes.size() ; ++i) {
    ret += flightTimes[i];
  }
  return ret;
}
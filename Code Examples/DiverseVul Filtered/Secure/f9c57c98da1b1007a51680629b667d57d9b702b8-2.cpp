void accountFlightTime(qids_t::const_iterator iter)
{
  if(flightTimes.empty())
    flightTimes.resize(2050); 

  struct timeval now;
  gettimeofday(&now, 0);
  unsigned int mdiff = 1000*DiffTime(iter->d_resentTime, now);
  if(mdiff > flightTimes.size()-2)
    mdiff= flightTimes.size()-1;

  flightTimes[mdiff]++;
}
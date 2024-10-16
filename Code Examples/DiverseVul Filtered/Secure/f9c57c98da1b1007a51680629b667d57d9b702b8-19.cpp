void houseKeeping()
{
  static timeval last;

  struct timeval now;
  gettimeofday(&now, 0);

  if(DiffTime(last, now) < 0.3)
    return;

  int weWaitingFor, origWaitingFor, weSlow, origSlow;
  WeOrigSlowQueriesDelta(weWaitingFor, origWaitingFor, weSlow, origSlow);
    
  if(!g_throttled) {
    if( weWaitingFor > 1000) {
      cerr<<"Too many questions ("<<weWaitingFor<<") outstanding, throttling"<<endl;
      g_throttled=true;
    }
  }
  else if(weWaitingFor < 750) {
    cerr<<"Unthrottling ("<<weWaitingFor<<")"<<endl;
    g_throttled=false;
  }

  if(DiffTime(last, now) < 2)
    return;

  last=now;

  /*
        Questions - Pend. - Drop = Answers = (On time + Late) = (Err + Ok)
Orig    9           21      29     36         47        57       66    72


   */

  printStats(origWaitingFor, weWaitingFor);
  pruneQids();
}
void WeOrigSlowQueriesDelta(int& weOutstanding, int& origOutstanding, int& weSlow, int& origSlow)
{
  struct timeval now;
  gettimeofday(&now, 0);

  weOutstanding=origOutstanding=weSlow=origSlow=0;

  for(qids_t::iterator i=qids.begin(); i!=qids.end(); ++i) {
    double dt=DiffTime(i->d_resentTime, now);
    if(dt < 2.0) {
      if(i->d_newRcode == -1) 
        weOutstanding++;
      if(i->d_origRcode == -1)
        origOutstanding++;
    }
    else {
      if(i->d_newRcode == -1) {
        weSlow++;
        if(!i->d_newlate) {
          QuestionData qd=*i;
          qd.d_newlate=true;
          qids.replace(i, qd);

          s_wetimedout++;
        }
      }
      if(i->d_origRcode == -1) {
        origSlow++;
        if(!i->d_origlate) {
          QuestionData qd=*i;
          qd.d_origlate=true;
          qids.replace(i, qd);

          s_origtimedout++;
        }
      }
    }
  }
}
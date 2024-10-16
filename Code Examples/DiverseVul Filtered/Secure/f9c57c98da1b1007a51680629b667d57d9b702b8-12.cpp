void pruneQids()
{
  struct timeval now;
  gettimeofday(&now, 0);

  for(qids_t::iterator i=qids.begin(); i!=qids.end(); ) {
    if(DiffTime(i->d_resentTime, now) < 10)
      ++i;
    else {
      s_idmanager.releaseID(i->d_assignedID);
      if(i->d_newRcode==-1) {
       s_wenever++;
      }
      if(i->d_origRcode==-1) {
        s_orignever++;
      }

      qids.erase(i++);
    }
  }
}
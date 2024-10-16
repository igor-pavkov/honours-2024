void measureResultAndClean(qids_t::const_iterator iter)
{
  const QuestionData& qd=*iter;
  accountFlightTime(iter);

  set<DNSRecord> canonicOrig, canonicNew;
  compactAnswerSet(qd.d_origAnswers, canonicOrig);
  compactAnswerSet(qd.d_newAnswers, canonicNew);
        
  if(!g_quiet) {
    cout<<qd.d_qi<<", orig rcode: "<<qd.d_origRcode<<", ours: "<<qd.d_newRcode;  
    cout<<", "<<canonicOrig.size()<< " vs " << canonicNew.size()<<", perfect: ";
  }

  if(canonicOrig==canonicNew) {
    s_perfect++;
    if(!g_quiet)
      cout<<"yes\n";
  }
  else {
    if(!g_quiet)
      cout<<"no\n";
    
    if(qd.d_norecursionavailable)
      if(!g_quiet)
        cout<<"\t* original nameserver did not provide recursion for this question *"<<endl;
    if(qd.d_origRcode == qd.d_newRcode ) {
      if(!g_quiet)
        cout<<"\t* mostly correct *"<<endl;
      s_mostly++;
    }

    if(!isRcodeOk(qd.d_origRcode) && isRcodeOk(qd.d_newRcode)) {
      if(!g_quiet)
        cout<<"\t* we better *"<<endl;
      s_webetter++;
    }
    if(isRcodeOk(qd.d_origRcode) && !isRcodeOk(qd.d_newRcode) && !isRootReferral(qd.d_origAnswers)) {
      if(!g_quiet)
        cout<<"\t* orig better *"<<endl;
      s_origbetter++;
      if(!g_quiet) 
        if(s_origbetterset.insert(make_pair(qd.d_qi.d_qname, qd.d_qi.d_qtype)).second) {
          cout<<"orig better: " << qd.d_qi.d_qname<<" "<< qd.d_qi.d_qtype<<endl;
        }
    }

    if(!g_quiet) {
      cout<<"orig: rcode="<<qd.d_origRcode<<"\n";
      for(set<DNSRecord>::const_iterator i=canonicOrig.begin(); i!=canonicOrig.end(); ++i)
        cout<<"\t"<<i->d_name<<"\t"<<DNSRecordContent::NumberToType(i->d_type)<<"\t'"  << (i->d_content ? i->d_content->getZoneRepresentation() : "") <<"'\n";
      cout<<"new: rcode="<<qd.d_newRcode<<"\n";
      for(set<DNSRecord>::const_iterator i=canonicNew.begin(); i!=canonicNew.end(); ++i)
        cout<<"\t"<<i->d_name<<"\t"<<DNSRecordContent::NumberToType(i->d_type)<<"\t'"  << (i->d_content ? i->d_content->getZoneRepresentation() : "") <<"'\n";
      cout<<"\n";
      cout<<"-\n";

    }
  }
  
  int releaseID=qd.d_assignedID;
  qids.erase(iter); // qd invalid now
  s_idmanager.releaseID(releaseID);
}
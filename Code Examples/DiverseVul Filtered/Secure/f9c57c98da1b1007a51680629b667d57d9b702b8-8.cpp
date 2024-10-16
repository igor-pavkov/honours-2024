bool sendPacketFromPR(PcapPacketReader& pr, const ComboAddress& remote, int stamp)
{
  dnsheader* dh=(dnsheader*)pr.d_payload;
  bool sent=false;
  if((ntohs(pr.d_udp->uh_dport)!=53 && ntohs(pr.d_udp->uh_sport)!=53) || dh->rd != g_rdSelector || (unsigned int)pr.d_len <= sizeof(dnsheader))
    return sent;

  QuestionData qd;
  try {
    // yes, we send out ALWAYS. Even if we don't do anything with it later, 
    if(!dh->qr) { // this is to stress out the reference server with all the pain
      s_questions++;
      qd.d_assignedID = s_idmanager.getID();
      uint16_t tmp=dh->id;
      dh->id=htons(qd.d_assignedID);
      //      dh->rd=1; // useful to replay traffic to auths to a recursor
      uint16_t dlen = pr.d_len;

      if (stamp >= 0) addECSOption((char*)pr.d_payload, 1500, &dlen, pr.getSource(), stamp);
      pr.d_len=dlen;
      s_socket->sendTo((const char*)pr.d_payload, dlen, remote);
      sent=true;
      dh->id=tmp;
    }
    MOADNSParser mdp(false, (const char*)pr.d_payload, pr.d_len);
    QuestionIdentifier qi=QuestionIdentifier::create(pr.getSource(), pr.getDest(), mdp);

    if(!mdp.d_header.qr) {

      if(qids.count(qi)) {
        if(!g_quiet)
          cout<<"Saw an exact duplicate question in PCAP "<<qi<< endl;
        s_duplicates++;
	s_idmanager.releaseID(qd.d_assignedID); // release = puts at back of pool
        return sent;
      }
      // new question - ID assigned above already
      qd.d_qi=qi;
      gettimeofday(&qd.d_resentTime,0);
      qids.insert(qd);
    }
    else {
      s_origanswers++;
      qids_t::const_iterator iter=qids.find(qi);      
      if(iter != qids.end()) {
        QuestionData eqd=*iter;
        eqd.d_origAnswers=mdp.d_answers;
        eqd.d_origRcode=mdp.d_header.rcode;
        
        if(!dh->ra) {
          s_norecursionavailable++;
          eqd.d_norecursionavailable=true;
        }
        qids.replace(iter, eqd);

        if(eqd.d_newRcode!=-1) {
          measureResultAndClean(iter);
        }
        
        return sent;
      }
      else {
        s_origunmatched++;
        if(!g_quiet)
          cout<<"Unmatched original answer "<<qi<<endl;
      }
    }
  }
  catch(MOADNSException &e)
  {
    if(!g_quiet)
      cerr<<"Error parsing packet: "<<e.what()<<endl;
    s_idmanager.releaseID(qd.d_assignedID);  // not added to qids for cleanup
    s_origdnserrors++;
  }
  catch(std::exception &e)
  {
    if(!g_quiet)
      cerr<<"Error parsing packet: "<<e.what()<<endl;

    s_idmanager.releaseID(qd.d_assignedID);  // not added to qids for cleanup
    s_origdnserrors++;    
  }

  return sent;
}
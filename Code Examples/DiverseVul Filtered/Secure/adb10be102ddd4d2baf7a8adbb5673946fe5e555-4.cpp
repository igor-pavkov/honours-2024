  void toPacket(DNSPacketWriter& pw)
  {
    string tmp((char*)&*d_record.begin(), d_record.size());
    vector<string> parts;
    stringtok(parts, tmp);
    if(parts.size()!=3 && !(parts.size()==2 && equals(parts[1],"0")) )
      throw MOADNSException("Unknown record was stored incorrectly, need 3 fields, got "+lexical_cast<string>(parts.size())+": "+tmp );
    const string& relevant=(parts.size() > 2) ? parts[2] : "";
    unsigned int total=atoi(parts[1].c_str());
    if(relevant.size()!=2*total)
      throw MOADNSException((boost::format("invalid unknown record length for label %s: size not equal to length field (%d != %d)") % d_dr.d_label.c_str() % relevant.size() % (2*total)).str());
    string out;
    out.reserve(total+1);
    for(unsigned int n=0; n < total; ++n) {
      int c;
      sscanf(relevant.c_str()+2*n, "%02x", &c);
      out.append(1, (char)c);
    }
    pw.xfrBlob(out);
  }
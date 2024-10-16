void ageDNSPacket(std::string& packet, uint32_t seconds)
{
  if(packet.length() < sizeof(dnsheader))
    return;
  try 
  {
    dnsheader dh;
    memcpy((void*)&dh, (const dnsheader*)packet.c_str(), sizeof(dh));
    int numrecords = ntohs(dh.ancount) + ntohs(dh.nscount) + ntohs(dh.arcount);
    DNSPacketMangler dpm(packet);
    
    int n;
    for(n=0; n < ntohs(dh.qdcount) ; ++n) {
      dpm.skipLabel();
      dpm.skipBytes(4); // qtype, qclass
    }
   // cerr<<"Skipped "<<n<<" questions, now parsing "<<numrecords<<" records"<<endl;
    for(n=0; n < numrecords; ++n) {
      dpm.skipLabel();
      
      uint16_t dnstype = dpm.get16BitInt();
      /* uint16_t dnsclass = */ dpm.get16BitInt();
      
      if(dnstype == QType::OPT) // not aging that one with a stick
        break;
      
      dpm.decreaseAndSkip32BitInt(seconds);
      dpm.skipRData();
    }
  }
  catch(...)
  {
    return;
  }
}
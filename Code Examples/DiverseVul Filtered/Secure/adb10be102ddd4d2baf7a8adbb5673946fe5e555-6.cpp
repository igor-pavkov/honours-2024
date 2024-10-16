DNSRecordContent* DNSRecordContent::mastermake(uint16_t qtype, uint16_t qclass,
                                               const string& content)
{
  zmakermap_t::const_iterator i=getZmakermap().find(make_pair(qclass, qtype));
  if(i==getZmakermap().end()) {
    return new UnknownRecordContent(content);
  }

  return i->second(content);
}
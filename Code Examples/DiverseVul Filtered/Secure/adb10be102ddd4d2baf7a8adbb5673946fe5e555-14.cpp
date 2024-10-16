DNSRecordContent* DNSRecordContent::mastermake(const DNSRecord &dr, 
                                               PacketReader& pr)
{
  uint16_t searchclass = (dr.d_type == QType::OPT) ? 1 : dr.d_class; // class is invalid for OPT

  typemap_t::const_iterator i=getTypemap().find(make_pair(searchclass, dr.d_type));
  if(i==getTypemap().end() || !i->second) {
    return new UnknownRecordContent(dr, pr);
  }

  return i->second(dr, pr);
}
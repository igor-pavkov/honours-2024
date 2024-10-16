DNSRecordContent* DNSRecordContent::mastermake(const DNSRecord &dr, PacketReader& pr, uint16_t oc) {
  // For opcode UPDATE and where the DNSRecord is an answer record, we don't care about content, because this is
  // not used within the prerequisite section of RFC2136, so - we can simply use unknownrecordcontent.
  // For section 3.2.3, we do need content so we need to get it properly. But only for the correct Qclasses.
  if (oc == Opcode::Update && dr.d_place == DNSRecord::Answer && dr.d_class != 1)
    return new UnknownRecordContent(dr, pr);

  uint16_t searchclass = (dr.d_type == QType::OPT) ? 1 : dr.d_class; // class is invalid for OPT

  typemap_t::const_iterator i=getTypemap().find(make_pair(searchclass, dr.d_type));
  if(i==getTypemap().end() || !i->second) {
    return new UnknownRecordContent(dr, pr);
  }

  return i->second(dr, pr);
}
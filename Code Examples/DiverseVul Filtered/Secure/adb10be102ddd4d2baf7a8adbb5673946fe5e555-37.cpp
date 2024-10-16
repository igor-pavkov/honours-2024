  UnknownRecordContent(const DNSRecord& dr, PacketReader& pr) 
    : DNSRecordContent(dr.d_type), d_dr(dr)
  {
    pr.copyRecord(d_record, dr.d_clen);
  }
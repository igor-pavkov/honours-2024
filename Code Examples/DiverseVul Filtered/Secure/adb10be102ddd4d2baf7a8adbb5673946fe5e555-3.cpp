  UnknownRecordContent(const string& zone) : DNSRecordContent(0)
  {
    d_record.insert(d_record.end(), zone.begin(), zone.end());
  }
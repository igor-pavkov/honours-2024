DNSRecordContent::t2namemap_t& DNSRecordContent::getT2Namemap()
{
  static DNSRecordContent::t2namemap_t t2namemap;
  return t2namemap;
}
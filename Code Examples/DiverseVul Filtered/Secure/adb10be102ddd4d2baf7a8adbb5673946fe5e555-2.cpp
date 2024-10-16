DNSRecordContent::typemap_t& DNSRecordContent::getTypemap()
{
  static DNSRecordContent::typemap_t typemap;
  return typemap;
}
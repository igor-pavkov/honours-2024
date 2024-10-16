DNSRecordContent::n2typemap_t& DNSRecordContent::getN2Typemap()
{
  static DNSRecordContent::n2typemap_t n2typemap;
  return n2typemap;
}
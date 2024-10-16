static void dump_bucket_metadata(struct req_state *s, rgw::sal::RGWBucket* bucket)
{
  dump_header(s, "X-RGW-Object-Count", static_cast<long long>(bucket->get_count()));
  dump_header(s, "X-RGW-Bytes-Used", static_cast<long long>(bucket->get_size()));
}
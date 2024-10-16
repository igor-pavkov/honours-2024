static void dump_bucket_metadata(struct req_state *s, RGWBucketEnt& bucket)
{
  dump_header(s, "X-RGW-Object-Count", static_cast<long long>(bucket.count));
  dump_header(s, "X-RGW-Bytes-Used", static_cast<long long>(bucket.size));
}
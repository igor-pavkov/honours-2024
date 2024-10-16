static inline std::string get_s3_expiration_header(
  struct req_state* s,
  const ceph::real_time& mtime)
{
  return rgw::lc::s3_expiration_header(
    s, s->object, s->tagset, mtime, s->bucket_attrs);
}
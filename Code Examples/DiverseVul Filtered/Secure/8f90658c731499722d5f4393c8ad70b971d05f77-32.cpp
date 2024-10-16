int RGWHandler_REST_S3::init_from_header(struct req_state* s,
					int default_formatter,
					bool configurable_format)
{
  string req;
  string first;

  const char *req_name = s->relative_uri.c_str();
  const char *p;

  if (*req_name == '?') {
    p = req_name;
  } else {
    p = s->info.request_params.c_str();
  }

  s->info.args.set(p);
  s->info.args.parse();

  /* must be called after the args parsing */
  int ret = allocate_formatter(s, default_formatter, configurable_format);
  if (ret < 0)
    return ret;

  if (*req_name != '/')
    return 0;

  req_name++;

  if (!*req_name)
    return 0;

  req = req_name;
  int pos = req.find('/');
  if (pos >= 0) {
    first = req.substr(0, pos);
  } else {
    first = req;
  }

  /*
   * XXX The intent of the check for empty is apparently to let the bucket
   * name from DNS to be set ahead. However, we currently take the DNS
   * bucket and re-insert it into URL in rgw_rest.cc:RGWREST::preprocess().
   * So, this check is meaningless.
   *
   * Rather than dropping this, the code needs to be changed into putting
   * the bucket (and its tenant) from DNS and Host: header (HTTP_HOST)
   * into req_status.bucket_name directly.
   */
  if (s->init_state.url_bucket.empty()) {
    // Save bucket to tide us over until token is parsed.
    s->init_state.url_bucket = first;
    if (pos >= 0) {
      string encoded_obj_str = req.substr(pos+1);
      s->object = rgw_obj_key(encoded_obj_str, s->info.args.get("versionId"));
    }
  } else {
    s->object = rgw_obj_key(req_name, s->info.args.get("versionId"));
  }
  return 0;
}
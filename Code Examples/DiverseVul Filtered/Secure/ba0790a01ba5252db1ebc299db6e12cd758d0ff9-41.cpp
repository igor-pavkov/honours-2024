int RGWSetBucketWebsite_ObjStore_S3::get_params()
{
  char *data = nullptr;
  int len = 0;
  const auto max_size = s->cct->_conf->rgw_max_put_param_size;
  int r = rgw_rest_read_all_input(s, &data, &len, max_size, false);

  if (r < 0) {
    return r;
  }

  auto data_deleter = std::unique_ptr<char, decltype(free)*>{data, free};

  r = do_aws4_auth_completion();
  if (r < 0) {
    return r;
  }

  bufferptr in_ptr(data, len);
  in_data.append(in_ptr);

  RGWXMLDecoder::XMLParser parser;
  if (!parser.init()) {
    ldout(s->cct, 0) << "ERROR: failed to initialize parser" << dendl;
    return -EIO;
  }

  if (!parser.parse(data, len, 1)) {
    string str(data, len);
    ldout(s->cct, 5) << "failed to parse xml: " << str << dendl;
    return -EINVAL;
  }

  try {
    RGWXMLDecoder::decode_xml("WebsiteConfiguration", website_conf, &parser, true);
  } catch (RGWXMLDecoder::err& err) {
    string str(data, len);
    ldout(s->cct, 5) << "unexpected xml: " << str << dendl;
    return -EINVAL;
  }

  if (website_conf.is_redirect_all && website_conf.redirect_all.hostname.empty()) {
    s->err.message = "A host name must be provided to redirect all requests (e.g. \"example.com\").";
    ldout(s->cct, 5) << s->err.message << dendl;
    return -EINVAL;
  } else if (!website_conf.is_redirect_all && !website_conf.is_set_index_doc) {
    s->err.message = "A value for IndexDocument Suffix must be provided if RedirectAllRequestsTo is empty";
    ldout(s->cct, 5) << s->err.message << dendl;
    return -EINVAL;
  } else if (!website_conf.is_redirect_all && website_conf.is_set_index_doc &&
             website_conf.index_doc_suffix.empty()) {
    s->err.message = "The IndexDocument Suffix is not well formed";
    ldout(s->cct, 5) << s->err.message << dendl;
    return -EINVAL;
  }

  return 0;
}
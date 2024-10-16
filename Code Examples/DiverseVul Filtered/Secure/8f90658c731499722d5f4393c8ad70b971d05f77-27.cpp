int RGWPutBucketReplication_ObjStore_S3::get_params()
{
  RGWXMLParser parser;

  if (!parser.init()){
    return -EINVAL;
  }

  const auto max_size = s->cct->_conf->rgw_max_put_param_size;
  int r = 0;
  bufferlist data;

  std::tie(r, data) = rgw_rest_read_all_input(s, max_size, false);

  if (r < 0)
    return r;

  if (!parser.parse(data.c_str(), data.length(), 1)) {
    return -ERR_MALFORMED_XML;
  }

  ReplicationConfiguration conf;
  try {
    RGWXMLDecoder::decode_xml("ReplicationConfiguration", conf, &parser);
  } catch (RGWXMLDecoder::err& err) {
    
    ldout(s->cct, 5) << "Malformed tagging request: " << err << dendl;
    return -ERR_MALFORMED_XML;
  }

  r = conf.to_sync_policy_groups(s, store, &sync_policy_groups);
  if (r < 0) {
    return r;
  }

  // forward requests to meta master zone
  if (!store->svc()->zone->is_meta_master()) {
    /* only need to keep this data around if we're not meta master */
    in_data = std::move(data);
  }

  return 0;
}
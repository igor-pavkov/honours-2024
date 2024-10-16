int RGWPutBucketTags_ObjStore_S3::get_params()
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

  RGWObjTagging_S3 tagging;
  try {
    RGWXMLDecoder::decode_xml("Tagging", tagging, &parser);
  } catch (RGWXMLDecoder::err& err) {
    
    ldout(s->cct, 5) << "Malformed tagging request: " << err << dendl;
    return -ERR_MALFORMED_XML;
  }

  RGWObjTags obj_tags(50); // A tag set can contain as many as 50 tags, or it can be empty.
  r = tagging.rebuild(obj_tags);
  if (r < 0)
    return r;

  obj_tags.encode(tags_bl);
  ldout(s->cct, 20) << "Read " << obj_tags.count() << "tags" << dendl;

  // forward bucket tags requests to meta master zone
  if (!store->svc()->zone->is_meta_master()) {
    /* only need to keep this data around if we're not meta master */
    in_data = std::move(data);
  }

  return 0;
}
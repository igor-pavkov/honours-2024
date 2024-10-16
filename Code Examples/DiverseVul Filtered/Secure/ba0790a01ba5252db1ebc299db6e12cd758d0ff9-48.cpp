int RGWPutObjTags_ObjStore_S3::get_params()
{
  RGWObjTagsXMLParser parser;

  if (!parser.init()){
    return -EINVAL;
  }

  char *data=nullptr;
  int len=0;

  const auto max_size = s->cct->_conf->rgw_max_put_param_size;
  int r = rgw_rest_read_all_input(s, &data, &len, max_size, false);

  if (r < 0)
    return r;

  auto data_deleter = std::unique_ptr<char, decltype(free)*>{data, free};

  if (!parser.parse(data, len, 1)) {
    return -ERR_MALFORMED_XML;
  }

  RGWObjTagSet_S3 *obj_tags_s3;
  RGWObjTagging_S3 *tagging;

  tagging = static_cast<RGWObjTagging_S3 *>(parser.find_first("Tagging"));
  obj_tags_s3 = static_cast<RGWObjTagSet_S3 *>(tagging->find_first("TagSet"));
  if(!obj_tags_s3){
    return -ERR_MALFORMED_XML;
  }

  RGWObjTags obj_tags;
  r = obj_tags_s3->rebuild(obj_tags);
  if (r < 0)
    return r;

  obj_tags.encode(tags_bl);
  ldout(s->cct, 20) << "Read " << obj_tags.count() << "tags" << dendl;

  return 0;
}
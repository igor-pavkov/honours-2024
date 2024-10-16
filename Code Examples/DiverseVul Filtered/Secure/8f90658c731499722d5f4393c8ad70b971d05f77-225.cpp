void RGWGetBucketMetaSearch_ObjStore_S3::send_response()
{
  if (op_ret)
    set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, NULL, "application/xml");

  Formatter *f = s->formatter;
  f->open_array_section("GetBucketMetaSearchResult");
  for (auto& e : s->bucket_info.mdsearch_config) {
    f->open_object_section("Entry");
    string k = string("x-amz-meta-") + e.first;
    f->dump_string("Key", k.c_str());
    const char *type;
    switch (e.second) {
      case ESEntityTypeMap::ES_ENTITY_INT:
        type = "int";
        break;
      case ESEntityTypeMap::ES_ENTITY_DATE:
        type = "date";
        break;
      default:
        type = "str";
    }
    f->dump_string("Type", type);
    f->close_section();
  }
  f->close_section();
  rgw_flush_formatter(s, f);
}
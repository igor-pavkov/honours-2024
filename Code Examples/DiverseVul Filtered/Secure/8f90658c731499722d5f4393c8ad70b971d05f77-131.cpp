void RGWDeleteMultiObj_ObjStore_S3::send_partial_response(rgw_obj_key& key,
							  bool delete_marker,
							  const string& marker_version_id, int ret)
{
  if (!key.empty()) {
    if (ret == 0 && !quiet) {
      s->formatter->open_object_section("Deleted");
      s->formatter->dump_string("Key", key.name);
      if (!key.instance.empty()) {
	s->formatter->dump_string("VersionId", key.instance);
      }
      if (delete_marker) {
	s->formatter->dump_bool("DeleteMarker", true);
	s->formatter->dump_string("DeleteMarkerVersionId", marker_version_id);
      }
      s->formatter->close_section();
    } else if (ret < 0) {
      struct rgw_http_error r;
      int err_no;

      s->formatter->open_object_section("Error");

      err_no = -ret;
      rgw_get_errno_s3(&r, err_no);

      s->formatter->dump_string("Key", key.name);
      s->formatter->dump_string("VersionId", key.instance);
      s->formatter->dump_string("Code", r.s3_code);
      s->formatter->dump_string("Message", r.s3_code);
      s->formatter->close_section();
    }

    rgw_flush_formatter(s, s->formatter);
  }
}
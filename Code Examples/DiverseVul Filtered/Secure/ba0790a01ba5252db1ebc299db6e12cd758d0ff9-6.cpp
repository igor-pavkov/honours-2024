void RGWListBucket_ObjStore_S3::send_versioned_response()
{
  s->formatter->open_object_section_in_ns("ListVersionsResult", XMLNS_AWS_S3);
  if (!s->bucket_tenant.empty())
    s->formatter->dump_string("Tenant", s->bucket_tenant);
  s->formatter->dump_string("Name", s->bucket_name);
  s->formatter->dump_string("Prefix", prefix);
  s->formatter->dump_string("KeyMarker", marker.name);
  s->formatter->dump_string("VersionIdMarker", marker.instance);
  if (is_truncated && !next_marker.empty()) {
    s->formatter->dump_string("NextKeyMarker", next_marker.name);
    s->formatter->dump_string("NextVersionIdMarker", next_marker.instance);
  }
  s->formatter->dump_int("MaxKeys", max);
  if (!delimiter.empty())
    s->formatter->dump_string("Delimiter", delimiter);

  s->formatter->dump_string("IsTruncated", (max && is_truncated ? "true"
					    : "false"));

  bool encode_key = false;
  if (strcasecmp(encoding_type.c_str(), "url") == 0) {
    s->formatter->dump_string("EncodingType", "url");
    encode_key = true;
  }

  if (op_ret >= 0) {
    if (objs_container) {
      s->formatter->open_array_section("Entries");
    }

    vector<rgw_bucket_dir_entry>::iterator iter;
    for (iter = objs.begin(); iter != objs.end(); ++iter) {
      const char *section_name = (iter->is_delete_marker() ? "DeleteMarker"
				  : "Version");
      s->formatter->open_object_section(section_name);
      if (objs_container) {
        s->formatter->dump_bool("IsDeleteMarker", iter->is_delete_marker());
      }
      rgw_obj_key key(iter->key);
      if (encode_key) {
	string key_name;
	url_encode(key.name, key_name);
	s->formatter->dump_string("Key", key_name);
      } else {
	s->formatter->dump_string("Key", key.name);
      }
      string version_id = key.instance;
      if (version_id.empty()) {
	version_id = "null";
      }
      if (s->system_request) {
        if (iter->versioned_epoch > 0) {
          s->formatter->dump_int("VersionedEpoch", iter->versioned_epoch);
        }
        s->formatter->dump_string("RgwxTag", iter->tag);
        utime_t ut(iter->meta.mtime);
        ut.gmtime_nsec(s->formatter->dump_stream("RgwxMtime"));
      }
      s->formatter->dump_string("VersionId", version_id);
      s->formatter->dump_bool("IsLatest", iter->is_current());
      dump_time(s, "LastModified", &iter->meta.mtime);
      if (!iter->is_delete_marker()) {
	s->formatter->dump_format("ETag", "\"%s\"", iter->meta.etag.c_str());
	s->formatter->dump_int("Size", iter->meta.accounted_size);
	s->formatter->dump_string("StorageClass", "STANDARD");
      }
      dump_owner(s, iter->meta.owner, iter->meta.owner_display_name);
      s->formatter->close_section();
    }
    if (objs_container) {
      s->formatter->close_section();
    }

    if (!common_prefixes.empty()) {
      map<string, bool>::iterator pref_iter;
      for (pref_iter = common_prefixes.begin();
	   pref_iter != common_prefixes.end(); ++pref_iter) {
	s->formatter->open_array_section("CommonPrefixes");
	s->formatter->dump_string("Prefix", pref_iter->first);
	s->formatter->close_section();
      }
    }
  }
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
}
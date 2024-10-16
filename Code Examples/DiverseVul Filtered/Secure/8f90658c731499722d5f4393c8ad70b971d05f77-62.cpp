void RGWListBucket_ObjStore_S3::send_common_versioned_response()
{
  if (!s->bucket_tenant.empty()) {
    s->formatter->dump_string("Tenant", s->bucket_tenant);
  }
  s->formatter->dump_string("Name", s->bucket_name);
  s->formatter->dump_string("Prefix", prefix);
  s->formatter->dump_int("MaxKeys", max);
  if (!delimiter.empty()) {
    s->formatter->dump_string("Delimiter", delimiter);
  }
  s->formatter->dump_string("IsTruncated", (max && is_truncated ? "true"
              : "false"));

  if (!common_prefixes.empty()) {
      map<string, bool>::iterator pref_iter;
      for (pref_iter = common_prefixes.begin();
      pref_iter != common_prefixes.end(); ++pref_iter) {
      s->formatter->open_array_section("CommonPrefixes");
      if (encode_key) {
        s->formatter->dump_string("Prefix", url_encode(pref_iter->first, false));
      } else {
        s->formatter->dump_string("Prefix", pref_iter->first);
      }

      s->formatter->close_section();
      }
    }
  }
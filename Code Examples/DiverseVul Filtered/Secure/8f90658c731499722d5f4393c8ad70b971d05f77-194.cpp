void RGWCompleteMultipart_ObjStore_S3::send_response()
{
  if (op_ret)
    set_req_state_err(s, op_ret);
  dump_errno(s);
  dump_header_if_nonempty(s, "x-amz-version-id", version_id);
  end_header(s, this, "application/xml");
  if (op_ret == 0) {
    dump_start(s);
    s->formatter->open_object_section_in_ns("CompleteMultipartUploadResult", XMLNS_AWS_S3);
    std::string base_uri = compute_domain_uri(s);
    if (!s->bucket_tenant.empty()) {
      s->formatter->dump_format("Location", "%s/%s:%s/%s",
	  base_uri.c_str(),
	  s->bucket_tenant.c_str(),
	  s->bucket_name.c_str(),
	  s->object.name.c_str()
          );
      s->formatter->dump_string("Tenant", s->bucket_tenant);
    } else {
      s->formatter->dump_format("Location", "%s/%s/%s",
	  base_uri.c_str(),
	  s->bucket_name.c_str(),
	  s->object.name.c_str()
          );
    }
    s->formatter->dump_string("Bucket", s->bucket_name);
    s->formatter->dump_string("Key", s->object.name);
    s->formatter->dump_string("ETag", etag);
    s->formatter->close_section();
    rgw_flush_formatter_and_reset(s, s->formatter);
  }
}
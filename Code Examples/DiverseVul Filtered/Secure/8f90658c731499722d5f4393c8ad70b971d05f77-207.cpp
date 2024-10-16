void RGWInitMultipart_ObjStore_S3::send_response()
{
  if (op_ret)
    set_req_state_err(s, op_ret);
  dump_errno(s);
  for (auto &it : crypt_http_responses)
     dump_header(s, it.first, it.second);
  end_header(s, this, "application/xml");
  if (op_ret == 0) {
    dump_start(s);
    s->formatter->open_object_section_in_ns("InitiateMultipartUploadResult", XMLNS_AWS_S3);
    if (!s->bucket_tenant.empty())
      s->formatter->dump_string("Tenant", s->bucket_tenant);
    s->formatter->dump_string("Bucket", s->bucket_name);
    s->formatter->dump_string("Key", s->object.name);
    s->formatter->dump_string("UploadId", upload_id);
    s->formatter->close_section();
    rgw_flush_formatter_and_reset(s, s->formatter);
  }
}
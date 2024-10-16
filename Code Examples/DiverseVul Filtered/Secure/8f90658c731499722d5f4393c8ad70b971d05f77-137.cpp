void RGWCopyObj_ObjStore_S3::send_response()
{
  if (!sent_header)
    send_partial_response(0);

  if (op_ret == 0) {
    dump_time(s, "LastModified", &mtime);
    if (!etag.empty()) {
      s->formatter->dump_string("ETag", std::move(etag));
    }
    s->formatter->close_section();
    rgw_flush_formatter_and_reset(s, s->formatter);
  }
}
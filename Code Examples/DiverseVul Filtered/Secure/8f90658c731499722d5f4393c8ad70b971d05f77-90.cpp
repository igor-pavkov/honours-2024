void RGWGetBucketLogging_ObjStore_S3::send_response()
{
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  s->formatter->open_object_section_in_ns("BucketLoggingStatus", XMLNS_AWS_S3);
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
}
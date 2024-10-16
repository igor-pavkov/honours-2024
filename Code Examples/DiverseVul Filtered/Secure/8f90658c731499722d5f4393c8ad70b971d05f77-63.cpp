void RGWGetBucketWebsite_ObjStore_S3::send_response()
{
  if (op_ret)
    set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  if (op_ret < 0) {
    return;
  }

  RGWBucketWebsiteConf& conf = s->bucket_info.website_conf;

  s->formatter->open_object_section_in_ns("WebsiteConfiguration", XMLNS_AWS_S3);
  conf.dump_xml(s->formatter);
  s->formatter->close_section(); // WebsiteConfiguration
  rgw_flush_formatter_and_reset(s, s->formatter);
}
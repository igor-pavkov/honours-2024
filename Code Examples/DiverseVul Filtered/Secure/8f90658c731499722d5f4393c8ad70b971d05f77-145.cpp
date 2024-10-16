void RGWGetBucketPolicyStatus_ObjStore_S3::send_response()
{
  if (op_ret) {
    set_req_state_err(s, op_ret);
  }
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  s->formatter->open_object_section_in_ns("PolicyStatus", XMLNS_AWS_S3);
  // https://docs.aws.amazon.com/AmazonS3/latest/API/RESTBucketGETPolicyStatus.html
  // mentions TRUE and FALSE, but boto/aws official clients seem to want lower
  // case which is returned by AWS as well; so let's be bug to bug compatible
  // with the API
  s->formatter->dump_bool("IsPublic", isPublic);
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);

}
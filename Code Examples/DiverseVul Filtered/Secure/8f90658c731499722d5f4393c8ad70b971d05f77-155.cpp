void RGWGetRequestPayment_ObjStore_S3::send_response()
{
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  s->formatter->open_object_section_in_ns("RequestPaymentConfiguration", XMLNS_AWS_S3);
  const char *payer = requester_pays ? "Requester" :  "BucketOwner";
  s->formatter->dump_string("Payer", payer);
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
}
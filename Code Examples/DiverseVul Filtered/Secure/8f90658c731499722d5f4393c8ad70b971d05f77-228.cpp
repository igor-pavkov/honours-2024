void RGWGetCORS_ObjStore_S3::send_response()
{
  if (op_ret) {
    if (op_ret == -ENOENT)
      set_req_state_err(s, ERR_NO_SUCH_CORS_CONFIGURATION);
    else
      set_req_state_err(s, op_ret);
  }
  dump_errno(s);
  end_header(s, NULL, "application/xml");
  dump_start(s);
  if (! op_ret) {
    string cors;
    RGWCORSConfiguration_S3 *s3cors =
      static_cast<RGWCORSConfiguration_S3 *>(&bucket_cors);
    stringstream ss;

    s3cors->to_xml(ss);
    cors = ss.str();
    dump_body(s, cors);
  }
}
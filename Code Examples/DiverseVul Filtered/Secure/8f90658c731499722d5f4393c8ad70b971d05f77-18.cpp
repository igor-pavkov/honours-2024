void RGWDeleteBucketWebsite_ObjStore_S3::send_response()
{
  if (op_ret == 0) {
    op_ret = STATUS_NO_CONTENT;
  }
  set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this, "application/xml");
}
void RGWStatBucket_ObjStore_S3::send_response()
{
  if (op_ret >= 0) {
    dump_bucket_metadata(s, bucket);
  }

  set_req_state_err(s, op_ret);
  dump_errno(s);

  end_header(s, this);
  dump_start(s);
}
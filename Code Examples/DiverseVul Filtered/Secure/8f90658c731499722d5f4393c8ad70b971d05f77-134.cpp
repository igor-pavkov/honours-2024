void RGWGetBucketObjectLock_ObjStore_S3::send_response()
{
  if (op_ret) {
    set_req_state_err(s, op_ret);
  }
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  if (op_ret) {
    return;
  }
  encode_xml("ObjectLockConfiguration", s->bucket_info.obj_lock, s->formatter);
  rgw_flush_formatter_and_reset(s, s->formatter);
}
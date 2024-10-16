void RGWDeleteObjTags_ObjStore_S3::send_response()
{
  int r = op_ret;
  if (r == -ENOENT)
    r = 0;
  if (!r)
    r = STATUS_NO_CONTENT;

  set_req_state_err(s, r);
  dump_errno(s);
  end_header(s, this);
}
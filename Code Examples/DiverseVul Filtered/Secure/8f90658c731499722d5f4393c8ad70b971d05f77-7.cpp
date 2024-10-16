void RGWDeleteObj_ObjStore_S3::send_response()
{
  int r = op_ret;
  if (r == -ENOENT)
    r = 0;
  if (!r)
    r = STATUS_NO_CONTENT;

  set_req_state_err(s, r);
  dump_errno(s);
  dump_header_if_nonempty(s, "x-amz-version-id", version_id);
  if (delete_marker) {
    dump_header(s, "x-amz-delete-marker", "true");
  }
  end_header(s, this);
}
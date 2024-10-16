void RGWDeleteMultiObj_ObjStore_S3::send_status()
{
  if (! status_dumped) {
    if (op_ret < 0)
      set_req_state_err(s, op_ret);
    dump_errno(s);
    status_dumped = true;
  }
}
void RGWGetLC_ObjStore_S3::send_response()
{
  if (op_ret) {
    if (op_ret == -ENOENT) {	
      set_req_state_err(s, ERR_NO_SUCH_LC);
    } else {
      set_req_state_err(s, op_ret);
    }
  }
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  if (op_ret < 0)
    return;

  encode_xml("LifecycleConfiguration", XMLNS_AWS_S3, config, s->formatter);
  rgw_flush_formatter_and_reset(s, s->formatter);
}
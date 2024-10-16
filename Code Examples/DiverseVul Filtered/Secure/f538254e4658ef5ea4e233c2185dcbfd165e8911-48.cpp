de265_error decoder_context::read_vps_NAL(bitreader& reader)
{
  logdebug(LogHeaders,"---> read VPS\n");

  std::shared_ptr<video_parameter_set> new_vps = std::make_shared<video_parameter_set>();
  de265_error err = new_vps->read(this,&reader);
  if (err != DE265_OK) {
    return err;
  }

  if (param_vps_headers_fd>=0) {
    new_vps->dump(param_vps_headers_fd);
  }

  vps[ new_vps->video_parameter_set_id ] = new_vps;

  return DE265_OK;
}
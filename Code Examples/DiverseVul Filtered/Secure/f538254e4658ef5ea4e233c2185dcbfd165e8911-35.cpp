de265_error decoder_context::read_sps_NAL(bitreader& reader)
{
  logdebug(LogHeaders,"----> read SPS\n");

  std::shared_ptr<seq_parameter_set> new_sps = std::make_shared<seq_parameter_set>();
  de265_error err;

  if ((err=new_sps->read(this, &reader)) != DE265_OK) {
    return err;
  }

  if (param_sps_headers_fd>=0) {
    new_sps->dump(param_sps_headers_fd);
  }

  sps[ new_sps->seq_parameter_set_id ] = new_sps;

  // Remove the all PPS that referenced the old SPS because parameters may have changed and we do not want to
  // get the SPS and PPS parameters (e.g. image size) out of sync.
  
  for (auto& p : pps) {
    if (p && p->seq_parameter_set_id == new_sps->seq_parameter_set_id) {
      p = nullptr;
    }
  }

  return DE265_OK;
}
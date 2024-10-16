int  decoder_context::get_highest_TID() const
{
  if (current_sps) { return current_sps->sps_max_sub_layers-1; }
  if (current_vps) { return current_vps->vps_max_sub_layers-1; }

  return 6;
}
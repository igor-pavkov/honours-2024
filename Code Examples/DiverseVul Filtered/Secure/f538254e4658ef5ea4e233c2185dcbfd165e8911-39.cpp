int decoder_context::generate_unavailable_reference_picture(const seq_parameter_set* sps,
                                                            int POC, bool longTerm)
{
  assert(dpb.has_free_dpb_picture(true));

  std::shared_ptr<const seq_parameter_set> current_sps = this->sps[ (int)current_pps->seq_parameter_set_id ];

  int idx = dpb.new_image(current_sps, this, 0,0, false);
  assert(idx>=0);
  //printf("-> fill with unavailable POC %d\n",POC);

  de265_image* img = dpb.get_image(idx);

  img->fill_image(1<<(sps->BitDepth_Y-1),
                  1<<(sps->BitDepth_C-1),
                  1<<(sps->BitDepth_C-1));

  img->fill_pred_mode(MODE_INTRA);

  img->PicOrderCntVal = POC;
  img->picture_order_cnt_lsb = POC & (sps->MaxPicOrderCntLsb-1);
  img->PicOutputFlag = false;
  img->PicState = (longTerm ? UsedForLongTermReference : UsedForShortTermReference);
  img->integrity = INTEGRITY_UNAVAILABLE_REFERENCE;

  return idx;
}
void decoder_context::init_thread_context(thread_context* tctx)
{
  // zero scrap memory for coefficient blocks
  memset(tctx->_coeffBuf, 0, sizeof(tctx->_coeffBuf));  // TODO: check if we can safely remove this

  tctx->currentQG_x = -1;
  tctx->currentQG_y = -1;



  // --- find QPY that was active at the end of the previous slice ---

  // find the previous CTB in TS order

  const pic_parameter_set& pps = tctx->img->get_pps();
  const seq_parameter_set& sps = tctx->img->get_sps();


  if (tctx->shdr->slice_segment_address > 0) {
    int prevCtb = pps.CtbAddrTStoRS[ pps.CtbAddrRStoTS[tctx->shdr->slice_segment_address] -1 ];

    int ctbX = prevCtb % sps.PicWidthInCtbsY;
    int ctbY = prevCtb / sps.PicWidthInCtbsY;


    // take the pixel at the bottom right corner (but consider that the image size might be smaller)

    int x = ((ctbX+1) << sps.Log2CtbSizeY)-1;
    int y = ((ctbY+1) << sps.Log2CtbSizeY)-1;

    x = std::min(x,sps.pic_width_in_luma_samples-1);
    y = std::min(y,sps.pic_height_in_luma_samples-1);

    //printf("READ QPY: %d %d -> %d (should %d)\n",x,y,imgunit->img->get_QPY(x,y), tc.currentQPY);

    //if (tctx->shdr->dependent_slice_segment_flag) {  // TODO: do we need this condition ?
    tctx->currentQPY = tctx->img->get_QPY(x,y);
      //}
  }
}
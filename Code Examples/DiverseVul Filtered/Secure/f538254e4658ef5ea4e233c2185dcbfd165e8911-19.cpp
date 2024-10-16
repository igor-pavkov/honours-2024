de265_error decoder_context::decode_slice_unit_sequential(image_unit* imgunit,
                                                          slice_unit* sliceunit)
{
  de265_error err = DE265_OK;

  /*
  printf("decode slice POC=%d addr=%d, img=%p\n",
         sliceunit->shdr->slice_pic_order_cnt_lsb,
         sliceunit->shdr->slice_segment_address,
         imgunit->img);
  */

  remove_images_from_dpb(sliceunit->shdr->RemoveReferencesList);

  if (sliceunit->shdr->slice_segment_address >= imgunit->img->get_pps().CtbAddrRStoTS.size()) {
    return DE265_ERROR_CTB_OUTSIDE_IMAGE_AREA;
  }


  struct thread_context tctx;

  tctx.shdr = sliceunit->shdr;
  tctx.img  = imgunit->img;
  tctx.decctx = this;
  tctx.imgunit = imgunit;
  tctx.sliceunit= sliceunit;
  tctx.CtbAddrInTS = imgunit->img->get_pps().CtbAddrRStoTS[tctx.shdr->slice_segment_address];
  tctx.task = NULL;

  init_thread_context(&tctx);

  if (sliceunit->reader.bytes_remaining <= 0) {
    return DE265_ERROR_PREMATURE_END_OF_SLICE;
  }

  init_CABAC_decoder(&tctx.cabac_decoder,
                     sliceunit->reader.data,
                     sliceunit->reader.bytes_remaining);

  // alloc CABAC-model array if entropy_coding_sync is enabled

  if (imgunit->img->get_pps().entropy_coding_sync_enabled_flag &&
      sliceunit->shdr->first_slice_segment_in_pic_flag) {
    imgunit->ctx_models.resize( (img->get_sps().PicHeightInCtbsY-1) ); //* CONTEXT_MODEL_TABLE_LENGTH );
  }

  sliceunit->nThreads=1;

  err=read_slice_segment_data(&tctx);

  sliceunit->finished_threads.set_progress(1);

  return err;
}
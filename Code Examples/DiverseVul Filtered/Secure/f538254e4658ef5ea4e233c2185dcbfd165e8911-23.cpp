void decoder_context::reset()
{
  if (num_worker_threads>0) {
    //flush_thread_pool(&ctx->thread_pool);
    ::stop_thread_pool(&thread_pool_);
  }

  // --------------------------------------------------

#if 0
  ctx->end_of_stream = false;
  ctx->pending_input_NAL = NULL;
  ctx->current_vps = NULL;
  ctx->current_sps = NULL;
  ctx->current_pps = NULL;
  ctx->num_worker_threads = 0;
  ctx->current_image_poc_lsb = 0;
  ctx->first_decoded_picture = 0;
  ctx->NoRaslOutputFlag = 0;
  ctx->HandleCraAsBlaFlag = 0;
  ctx->FirstAfterEndOfSequenceNAL = 0;
  ctx->PicOrderCntMsb = 0;
  ctx->prevPicOrderCntLsb = 0;
  ctx->prevPicOrderCntMsb = 0;
  ctx->NumPocStCurrBefore=0;
  ctx->NumPocStCurrAfter=0;
  ctx->NumPocStFoll=0;
  ctx->NumPocLtCurr=0;
  ctx->NumPocLtFoll=0;
  ctx->nal_unit_type=0;
  ctx->IdrPicFlag=0;
  ctx->RapPicFlag=0;
#endif

  img = NULL;


  // TODO: remove all pending image_units


  // --- decoded picture buffer ---

  current_image_poc_lsb = -1; // any invalid number
  first_decoded_picture = true;


  // --- remove all pictures from output queue ---

  // there was a bug the peek_next_image did not return NULL on empty output queues.
  // This was (indirectly) fixed by recreating the DPB buffer, but it should actually
  // be sufficient to clear it like this.
  // The error showed while scrubbing the ToS video in VLC.
  dpb.clear();

  nal_parser.remove_pending_input_data();


  while (!image_units.empty()) {
    delete image_units.back();
    image_units.pop_back();
  }

  // --- start threads again ---

  if (num_worker_threads>0) {
    // TODO: need error checking
    start_thread_pool(num_worker_threads);
  }
}
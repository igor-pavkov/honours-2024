de265_error decoder_context::decode(int* more)
{
  decoder_context* ctx = this;

  // if the stream has ended, and no more NALs are to be decoded, flush all pictures

  if (ctx->nal_parser.get_NAL_queue_length() == 0 &&
      (ctx->nal_parser.is_end_of_stream() || ctx->nal_parser.is_end_of_frame()) &&
      ctx->image_units.empty()) {

    // flush all pending pictures into output queue

    // ctx->push_current_picture_to_output_queue(); // TODO: not with new queue
    ctx->dpb.flush_reorder_buffer();

    if (more) { *more = ctx->dpb.num_pictures_in_output_queue(); }

    return DE265_OK;
  }


  // if NAL-queue is empty, we need more data
  // -> input stalled

  if (ctx->nal_parser.is_end_of_stream() == false &&
      ctx->nal_parser.is_end_of_frame() == false &&
      ctx->nal_parser.get_NAL_queue_length() == 0) {
    if (more) { *more=1; }

    return DE265_ERROR_WAITING_FOR_INPUT_DATA;
  }


  // when there are no free image buffers in the DPB, pause decoding
  // -> output stalled

  if (!ctx->dpb.has_free_dpb_picture(false)) {
    if (more) *more = 1;
    return DE265_ERROR_IMAGE_BUFFER_FULL;
  }


  // decode one NAL from the queue

  de265_error err = DE265_OK;
  bool did_work = false;

  if (ctx->nal_parser.get_NAL_queue_length()) { // number_of_NAL_units_pending()) {
    NAL_unit* nal = ctx->nal_parser.pop_from_NAL_queue();
    assert(nal);
    err = ctx->decode_NAL(nal);
    // ctx->nal_parser.free_NAL_unit(nal); TODO: do not free NAL with new loop
    did_work=true;
  }
  else if (ctx->nal_parser.is_end_of_frame() == true &&
      ctx->image_units.empty()) {
    if (more) { *more=1; }

    return DE265_ERROR_WAITING_FOR_INPUT_DATA;
  }
  else {
    err = decode_some(&did_work);
  }

  if (more) {
    // decoding error is assumed to be unrecoverable
    *more = (err==DE265_OK && did_work);
  }

  return err;
}
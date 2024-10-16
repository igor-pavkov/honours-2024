de265_error decoder_context::push_picture_to_output_queue(image_unit* imgunit)
{
  de265_image* outimg = imgunit->img;

  if (outimg==NULL) { return DE265_OK; }


  // push image into output queue

  if (outimg->PicOutputFlag) {
    loginfo(LogDPB,"new picture has output-flag=true\n");

    if (outimg->integrity != INTEGRITY_CORRECT &&
        param_suppress_faulty_pictures) {
    }
    else {
      dpb.insert_image_into_reorder_buffer(outimg);
    }

    loginfo(LogDPB,"push image %d into reordering queue\n", outimg->PicOrderCntVal);
  }

  // check for full reorder buffers

  int maxNumPicsInReorderBuffer = 0;

  // TODO: I'd like to have the has_vps() check somewhere else (not decode the picture at all)
  if (outimg->has_vps()) {
    int sublayer = outimg->get_vps().vps_max_sub_layers -1;
    maxNumPicsInReorderBuffer = outimg->get_vps().layer[sublayer].vps_max_num_reorder_pics;
  }

  if (dpb.num_pictures_in_reorder_buffer() > maxNumPicsInReorderBuffer) {
    dpb.output_next_picture_in_reorder_buffer();
  }

  dpb.log_dpb_queues();

  return DE265_OK;
}
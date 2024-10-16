de265_error decoder_context::decode_some(bool* did_work)
{
  de265_error err = DE265_OK;

  *did_work = false;

  if (image_units.empty()) { return DE265_OK; }  // nothing to do


  // decode something if there is work to do

  if ( ! image_units.empty() ) { // && ! image_units[0]->slice_units.empty() ) {

    image_unit* imgunit = image_units[0];
    slice_unit* sliceunit = imgunit->get_next_unprocessed_slice_segment();

    if (sliceunit != NULL) {

      //pop_front(imgunit->slice_units);

      if (sliceunit->flush_reorder_buffer) {
        dpb.flush_reorder_buffer();
      }

      *did_work = true;

      //err = decode_slice_unit_sequential(imgunit, sliceunit);
      err = decode_slice_unit_parallel(imgunit, sliceunit);
      if (err) {
        return err;
      }

      //delete sliceunit;
    }
  }



  // if we decoded all slices of the current image and there will not
  // be added any more slices to the image, output the image

  if ( ( image_units.size()>=2 && image_units[0]->all_slice_segments_processed()) ||
       ( image_units.size()>=1 && image_units[0]->all_slice_segments_processed() &&
         nal_parser.number_of_NAL_units_pending()==0 &&
         (nal_parser.is_end_of_stream() || nal_parser.is_end_of_frame()) )) {

    image_unit* imgunit = image_units[0];

    *did_work=true;


    // mark all CTBs as decoded even if they are not, because faulty input
    // streams could miss part of the picture
    // TODO: this will not work when slice decoding is parallel to post-filtering,
    // so we will have to replace this with keeping track of which CTB should have
    // been decoded (but aren't because of the input stream being faulty)

    imgunit->img->mark_all_CTB_progress(CTB_PROGRESS_PREFILTER);



    // run post-processing filters (deblocking & SAO)

    if (img->decctx->num_worker_threads)
      run_postprocessing_filters_parallel(imgunit);
    else
      run_postprocessing_filters_sequential(imgunit->img);

    // process suffix SEIs

    for (int i=0;i<imgunit->suffix_SEIs.size();i++) {
      const sei_message& sei = imgunit->suffix_SEIs[i];

      err = process_sei(&sei, imgunit->img);
      if (err != DE265_OK)
        break;
    }


    push_picture_to_output_queue(imgunit);

    // remove just decoded image unit from queue

    delete imgunit;

    pop_front(image_units);
  }

  return err;
}
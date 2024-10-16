de265_error decoder_context::read_slice_NAL(bitreader& reader, NAL_unit* nal, nal_header& nal_hdr)
{
  logdebug(LogHeaders,"---> read slice segment header\n");


  // --- read slice header ---

  slice_segment_header* shdr = new slice_segment_header;
  bool continueDecoding;
  de265_error err = shdr->read(&reader,this, &continueDecoding);
  if (!continueDecoding) {
    if (img) { img->integrity = INTEGRITY_NOT_DECODED; }
    nal_parser.free_NAL_unit(nal);
    delete shdr;
    return err;
  }

  if (param_slice_headers_fd>=0) {
    shdr->dump_slice_segment_header(this, param_slice_headers_fd);
  }


  if (process_slice_segment_header(shdr, &err, nal->pts, &nal_hdr, nal->user_data) == false)
    {
      if (img!=NULL) img->integrity = INTEGRITY_NOT_DECODED;
      nal_parser.free_NAL_unit(nal);
      delete shdr;
      return err;
    }

  this->img->add_slice_segment_header(shdr);

  skip_bits(&reader,1); // TODO: why?
  prepare_for_CABAC(&reader);


  // modify entry_point_offsets

  int headerLength = reader.data - nal->data();
  for (int i=0;i<shdr->num_entry_point_offsets;i++) {
    shdr->entry_point_offset[i] -= nal->num_skipped_bytes_before(shdr->entry_point_offset[i],
                                                                 headerLength);
  }



  // --- start a new image if this is the first slice ---

  if (shdr->first_slice_segment_in_pic_flag) {
    image_unit* imgunit = new image_unit;
    imgunit->img = this->img;
    image_units.push_back(imgunit);
  }


  // --- add slice to current picture ---

  if ( ! image_units.empty() ) {

    slice_unit* sliceunit = new slice_unit(this);
    sliceunit->nal = nal;
    sliceunit->shdr = shdr;
    sliceunit->reader = reader;

    sliceunit->flush_reorder_buffer = flush_reorder_buffer_at_this_frame;


    image_units.back()->slice_units.push_back(sliceunit);
  }

  bool did_work;
  err = decode_some(&did_work);

  return DE265_OK;
}
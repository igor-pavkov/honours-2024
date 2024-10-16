de265_error decoder_context::read_sei_NAL(bitreader& reader, bool suffix)
{
  logdebug(LogHeaders,"----> read SEI\n");

  sei_message sei;

  //push_current_picture_to_output_queue();

  de265_error err = DE265_OK;

  if ((err=read_sei(&reader,&sei, suffix, current_sps.get())) == DE265_OK) {
    dump_sei(&sei, current_sps.get());

    if (image_units.empty()==false && suffix) {
      image_units.back()->suffix_SEIs.push_back(sei);
    }
  }
  else {
    add_warning(err, false);
  }

  return err;
}
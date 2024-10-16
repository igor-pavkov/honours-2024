de265_error decoder_context::decode_NAL(NAL_unit* nal)
{
  //return decode_NAL_OLD(nal);

  decoder_context* ctx = this;

  de265_error err = DE265_OK;

  bitreader reader;
  bitreader_init(&reader, nal->data(), nal->size());

  nal_header nal_hdr;
  nal_hdr.read(&reader);
  ctx->process_nal_hdr(&nal_hdr);

  if (nal_hdr.nuh_layer_id > 0) {
    // Discard all NAL units with nuh_layer_id > 0
    // These will have to be handeled by an SHVC decoder.
    nal_parser.free_NAL_unit(nal);
    return DE265_OK;
  }

  loginfo(LogHighlevel,"NAL: 0x%x 0x%x -  unit type:%s temporal id:%d\n",
          nal->data()[0], nal->data()[1],
          get_NAL_name(nal_hdr.nal_unit_type),
          nal_hdr.nuh_temporal_id);

  /*
    printf("NAL: 0x%x 0x%x -  unit type:%s temporal id:%d\n",
    nal->data()[0], nal->data()[1],
    get_NAL_name(nal_hdr.nal_unit_type),
    nal_hdr.nuh_temporal_id);
  */

  // throw away NALs from higher TIDs than currently selected
  // TODO: better online switching of HighestTID

  //printf("hTid: %d\n", current_HighestTid);

  if (nal_hdr.nuh_temporal_id > current_HighestTid) {
    nal_parser.free_NAL_unit(nal);
    return DE265_OK;
  }


  if (nal_hdr.nal_unit_type<32) {
    err = read_slice_NAL(reader, nal, nal_hdr);
  }
  else switch (nal_hdr.nal_unit_type) {
    case NAL_UNIT_VPS_NUT:
      err = read_vps_NAL(reader);
      nal_parser.free_NAL_unit(nal);
      break;

    case NAL_UNIT_SPS_NUT:
      err = read_sps_NAL(reader);
      nal_parser.free_NAL_unit(nal);
      break;

    case NAL_UNIT_PPS_NUT:
      err = read_pps_NAL(reader);
      nal_parser.free_NAL_unit(nal);
      break;

    case NAL_UNIT_PREFIX_SEI_NUT:
    case NAL_UNIT_SUFFIX_SEI_NUT:
      err = read_sei_NAL(reader, nal_hdr.nal_unit_type==NAL_UNIT_SUFFIX_SEI_NUT);
      nal_parser.free_NAL_unit(nal);
      break;

    case NAL_UNIT_EOS_NUT:
      ctx->FirstAfterEndOfSequenceNAL = true;
      nal_parser.free_NAL_unit(nal);
      break;

    default:
      nal_parser.free_NAL_unit(nal);
      break;
    }

  return err;
}
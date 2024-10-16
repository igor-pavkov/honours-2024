void decoder_context::process_nal_hdr(nal_header* nal)
{
  nal_unit_type = nal->nal_unit_type;

  IdrPicFlag = isIdrPic(nal->nal_unit_type);
  RapPicFlag = isRapPic(nal->nal_unit_type);
}
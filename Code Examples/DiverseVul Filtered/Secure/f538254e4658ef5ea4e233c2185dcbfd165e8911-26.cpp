de265_error decoder_context::read_eos_NAL(bitreader& reader)
{
  FirstAfterEndOfSequenceNAL = true;
  return DE265_OK;
}
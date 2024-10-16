void BM_EndOfStream(void *co)
{
	((GF_BifsDecoder *) co)->LastError = GF_NON_COMPLIANT_BITSTREAM;
}
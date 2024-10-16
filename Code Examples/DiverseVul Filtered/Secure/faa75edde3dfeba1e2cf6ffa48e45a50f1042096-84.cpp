void lsr_end_of_stream(void *co)
{
	GF_LASeRCodec *codec = (GF_LASeRCodec *)co;
	if (codec->last_error != GF_NON_COMPLIANT_BITSTREAM) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CODING, ("[LASeR] memory overread - corrupted decoding\n"));
		((GF_LASeRCodec *) co)->last_error = GF_NON_COMPLIANT_BITSTREAM;
	}
}
FLAC_API const char *FLAC__stream_decoder_get_resolved_state_string(const FLAC__StreamDecoder *decoder)
{
	return FLAC__StreamDecoderStateString[decoder->protected_->state];
}
FLAC_API FLAC__bool FLAC__stream_decoder_get_md5_checking(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->protected_->md5_checking;
}
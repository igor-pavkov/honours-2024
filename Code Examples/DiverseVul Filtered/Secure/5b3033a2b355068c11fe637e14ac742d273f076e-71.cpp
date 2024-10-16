FLAC_API FLAC__uint64 FLAC__stream_decoder_get_total_samples(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->protected_);
	return decoder->private_->has_stream_info? decoder->private_->stream_info.data.stream_info.total_samples : 0;
}
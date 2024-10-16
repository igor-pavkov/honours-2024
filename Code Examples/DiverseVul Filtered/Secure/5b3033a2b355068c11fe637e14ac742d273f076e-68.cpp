FLAC_API FLAC__bool FLAC__stream_decoder_set_metadata_respond_all(FLAC__StreamDecoder *decoder)
{
	unsigned i;
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != decoder->protected_);
	if(decoder->protected_->state != FLAC__STREAM_DECODER_UNINITIALIZED)
		return false;
	for(i = 0; i < sizeof(decoder->private_->metadata_filter) / sizeof(decoder->private_->metadata_filter[0]); i++)
		decoder->private_->metadata_filter[i] = true;
	decoder->private_->metadata_filter_ids_count = 0;
	return true;
}
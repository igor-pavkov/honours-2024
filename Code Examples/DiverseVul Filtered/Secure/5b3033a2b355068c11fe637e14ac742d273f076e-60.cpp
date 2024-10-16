FLAC_API FLAC__bool FLAC__stream_decoder_set_metadata_ignore_application(FLAC__StreamDecoder *decoder, const FLAC__byte id[4])
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != decoder->protected_);
	FLAC__ASSERT(0 != id);
	if(decoder->protected_->state != FLAC__STREAM_DECODER_UNINITIALIZED)
		return false;

	if(!decoder->private_->metadata_filter[FLAC__METADATA_TYPE_APPLICATION])
		return true;

	FLAC__ASSERT(0 != decoder->private_->metadata_filter_ids);

	if(decoder->private_->metadata_filter_ids_count == decoder->private_->metadata_filter_ids_capacity) {
		if(0 == (decoder->private_->metadata_filter_ids = safe_realloc_mul_2op_(decoder->private_->metadata_filter_ids, decoder->private_->metadata_filter_ids_capacity, /*times*/2))) {
			decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
			return false;
		}
		decoder->private_->metadata_filter_ids_capacity *= 2;
	}

	memcpy(decoder->private_->metadata_filter_ids + decoder->private_->metadata_filter_ids_count * (FLAC__STREAM_METADATA_APPLICATION_ID_LEN/8), id, (FLAC__STREAM_METADATA_APPLICATION_ID_LEN/8));
	decoder->private_->metadata_filter_ids_count++;

	return true;
}
FLAC__bool has_id_filtered_(FLAC__StreamDecoder *decoder, FLAC__byte *id)
{
	size_t i;

	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);

	for(i = 0; i < decoder->private_->metadata_filter_ids_count; i++)
		if(0 == memcmp(decoder->private_->metadata_filter_ids + i * (FLAC__STREAM_METADATA_APPLICATION_ID_LEN/8), id, (FLAC__STREAM_METADATA_APPLICATION_ID_LEN/8)))
			return true;

	return false;
}
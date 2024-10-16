FLAC_API void FLAC__stream_decoder_delete(FLAC__StreamDecoder *decoder)
{
	unsigned i;

	if (decoder == NULL)
		return ;

	FLAC__ASSERT(0 != decoder->protected_);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != decoder->private_->input);

	(void)FLAC__stream_decoder_finish(decoder);

	if(0 != decoder->private_->metadata_filter_ids)
		free(decoder->private_->metadata_filter_ids);

	FLAC__bitreader_delete(decoder->private_->input);

	for(i = 0; i < FLAC__MAX_CHANNELS; i++)
		FLAC__format_entropy_coding_method_partitioned_rice_contents_clear(&decoder->private_->partitioned_rice_contents[i]);

	free(decoder->private_);
	free(decoder->protected_);
	free(decoder);
}
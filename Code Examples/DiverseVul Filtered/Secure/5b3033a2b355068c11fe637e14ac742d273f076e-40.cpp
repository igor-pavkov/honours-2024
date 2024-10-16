FLAC_API FLAC__StreamDecoder *FLAC__stream_decoder_new(void)
{
	FLAC__StreamDecoder *decoder;
	unsigned i;

	FLAC__ASSERT(sizeof(int) >= 4); /* we want to die right away if this is not true */

	decoder = calloc(1, sizeof(FLAC__StreamDecoder));
	if(decoder == 0) {
		return 0;
	}

	decoder->protected_ = calloc(1, sizeof(FLAC__StreamDecoderProtected));
	if(decoder->protected_ == 0) {
		free(decoder);
		return 0;
	}

	decoder->private_ = calloc(1, sizeof(FLAC__StreamDecoderPrivate));
	if(decoder->private_ == 0) {
		free(decoder->protected_);
		free(decoder);
		return 0;
	}

	decoder->private_->input = FLAC__bitreader_new();
	if(decoder->private_->input == 0) {
		free(decoder->private_);
		free(decoder->protected_);
		free(decoder);
		return 0;
	}

	decoder->private_->metadata_filter_ids_capacity = 16;
	if(0 == (decoder->private_->metadata_filter_ids = malloc((FLAC__STREAM_METADATA_APPLICATION_ID_LEN/8) * decoder->private_->metadata_filter_ids_capacity))) {
		FLAC__bitreader_delete(decoder->private_->input);
		free(decoder->private_);
		free(decoder->protected_);
		free(decoder);
		return 0;
	}

	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		decoder->private_->output[i] = 0;
		decoder->private_->residual_unaligned[i] = decoder->private_->residual[i] = 0;
	}

	decoder->private_->output_capacity = 0;
	decoder->private_->output_channels = 0;
	decoder->private_->has_seek_table = false;

	for(i = 0; i < FLAC__MAX_CHANNELS; i++)
		FLAC__format_entropy_coding_method_partitioned_rice_contents_init(&decoder->private_->partitioned_rice_contents[i]);

	decoder->private_->file = 0;

	set_defaults_(decoder);

	decoder->protected_->state = FLAC__STREAM_DECODER_UNINITIALIZED;

	return decoder;
}
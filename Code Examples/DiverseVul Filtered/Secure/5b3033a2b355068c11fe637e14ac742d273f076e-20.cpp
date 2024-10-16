static FLAC__StreamDecoderInitStatus init_file_internal_(
	FLAC__StreamDecoder *decoder,
	const char *filename,
	FLAC__StreamDecoderWriteCallback write_callback,
	FLAC__StreamDecoderMetadataCallback metadata_callback,
	FLAC__StreamDecoderErrorCallback error_callback,
	void *client_data,
	FLAC__bool is_ogg
)
{
	FILE *file;

	FLAC__ASSERT(0 != decoder);

	/*
	 * To make sure that our file does not go unclosed after an error, we
	 * have to do the same entrance checks here that are later performed
	 * in FLAC__stream_decoder_init_FILE() before the FILE* is assigned.
	 */
	if(decoder->protected_->state != FLAC__STREAM_DECODER_UNINITIALIZED)
		return decoder->protected_->initstate = FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED;

	if(0 == write_callback || 0 == error_callback)
		return decoder->protected_->initstate = FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS;

	file = filename? flac_fopen(filename, "rb") : stdin;

	if(0 == file)
		return FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE;

	return init_FILE_internal_(decoder, file, write_callback, metadata_callback, error_callback, client_data, is_ogg);
}
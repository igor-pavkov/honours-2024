static FLAC__StreamDecoderInitStatus init_FILE_internal_(
	FLAC__StreamDecoder *decoder,
	FILE *file,
	FLAC__StreamDecoderWriteCallback write_callback,
	FLAC__StreamDecoderMetadataCallback metadata_callback,
	FLAC__StreamDecoderErrorCallback error_callback,
	void *client_data,
	FLAC__bool is_ogg
)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != file);

	if(decoder->protected_->state != FLAC__STREAM_DECODER_UNINITIALIZED)
		return decoder->protected_->initstate = FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED;

	if(0 == write_callback || 0 == error_callback)
		return decoder->protected_->initstate = FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS;

	/*
	 * To make sure that our file does not go unclosed after an error, we
	 * must assign the FILE pointer before any further error can occur in
	 * this routine.
	 */
	if(file == stdin)
		file = get_binary_stdin_(); /* just to be safe */

	decoder->private_->file = file;

	return init_stream_internal_(
		decoder,
		file_read_callback_,
		decoder->private_->file == stdin? 0: file_seek_callback_,
		decoder->private_->file == stdin? 0: file_tell_callback_,
		decoder->private_->file == stdin? 0: file_length_callback_,
		file_eof_callback_,
		write_callback,
		metadata_callback,
		error_callback,
		client_data,
		is_ogg
	);
}
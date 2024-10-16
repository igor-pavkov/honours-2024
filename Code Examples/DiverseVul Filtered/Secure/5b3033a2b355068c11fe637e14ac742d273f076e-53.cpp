FLAC_API FLAC__StreamDecoderInitStatus FLAC__stream_decoder_init_file(
	FLAC__StreamDecoder *decoder,
	const char *filename,
	FLAC__StreamDecoderWriteCallback write_callback,
	FLAC__StreamDecoderMetadataCallback metadata_callback,
	FLAC__StreamDecoderErrorCallback error_callback,
	void *client_data
)
{
	return init_file_internal_(decoder, filename, write_callback, metadata_callback, error_callback, client_data, /*is_ogg=*/false);
}
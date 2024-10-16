FLAC__OggDecoderAspectReadStatus read_callback_proxy_(const void *void_decoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
	FLAC__StreamDecoder *decoder = (FLAC__StreamDecoder*)void_decoder;

	switch(decoder->private_->read_callback(decoder, buffer, bytes, client_data)) {
		case FLAC__STREAM_DECODER_READ_STATUS_CONTINUE:
			return FLAC__OGG_DECODER_ASPECT_READ_STATUS_OK;
		case FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM:
			return FLAC__OGG_DECODER_ASPECT_READ_STATUS_END_OF_STREAM;
		case FLAC__STREAM_DECODER_READ_STATUS_ABORT:
			return FLAC__OGG_DECODER_ASPECT_READ_STATUS_ABORT;
		default:
			/* double protection: */
			FLAC__ASSERT(0);
			return FLAC__OGG_DECODER_ASPECT_READ_STATUS_ABORT;
	}
}
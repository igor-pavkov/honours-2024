FLAC__StreamDecoderReadStatus read_callback_ogg_aspect_(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes)
{
	switch(FLAC__ogg_decoder_aspect_read_callback_wrapper(&decoder->protected_->ogg_decoder_aspect, buffer, bytes, read_callback_proxy_, decoder, decoder->private_->client_data)) {
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_OK:
			return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
		/* we don't really have a way to handle lost sync via read
		 * callback so we'll let it pass and let the underlying
		 * FLAC decoder catch the error
		 */
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_LOST_SYNC:
			return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_END_OF_STREAM:
			return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_NOT_FLAC:
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_UNSUPPORTED_MAPPING_VERSION:
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_ABORT:
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_ERROR:
		case FLAC__OGG_DECODER_ASPECT_READ_STATUS_MEMORY_ALLOCATION_ERROR:
			return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
		default:
			FLAC__ASSERT(0);
			/* double protection */
			return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	}
}
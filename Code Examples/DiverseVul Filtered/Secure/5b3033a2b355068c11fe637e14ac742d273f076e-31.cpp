FLAC_API FLAC__bool FLAC__stream_decoder_get_decode_position(const FLAC__StreamDecoder *decoder, FLAC__uint64 *position)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != position);

#if FLAC__HAS_OGG
	if(decoder->private_->is_ogg)
		return false;
#endif
	if(0 == decoder->private_->tell_callback)
		return false;
	if(decoder->private_->tell_callback(decoder, position, decoder->private_->client_data) != FLAC__STREAM_DECODER_TELL_STATUS_OK)
		return false;
	/* should never happen since all FLAC frames and metadata blocks are byte aligned, but check just in case */
	if(!FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input))
		return false;
	FLAC__ASSERT(*position >= FLAC__stream_decoder_get_input_bytes_unconsumed(decoder));
	*position -= FLAC__stream_decoder_get_input_bytes_unconsumed(decoder);
	return true;
}
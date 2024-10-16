unsigned FLAC__stream_decoder_get_input_bytes_unconsumed(const FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input));
	FLAC__ASSERT(!(FLAC__bitreader_get_input_bits_unconsumed(decoder->private_->input) & 7));
	return FLAC__bitreader_get_input_bits_unconsumed(decoder->private_->input) / 8;
}
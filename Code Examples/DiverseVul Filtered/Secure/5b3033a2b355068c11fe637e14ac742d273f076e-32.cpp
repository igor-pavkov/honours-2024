FLAC__bool read_subframe_constant_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode)
{
	FLAC__Subframe_Constant *subframe = &decoder->private_->frame.subframes[channel].data.constant;
	FLAC__int32 x;
	unsigned i;
	FLAC__int32 *output = decoder->private_->output[channel];

	decoder->private_->frame.subframes[channel].type = FLAC__SUBFRAME_TYPE_CONSTANT;

	if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &x, bps))
		return false; /* read_callback_ sets the state for us */

	subframe->value = x;

	/* decode the subframe */
	if(do_full_decode) {
		for(i = 0; i < decoder->private_->frame.header.blocksize; i++)
			output[i] = x;
	}

	return true;
}
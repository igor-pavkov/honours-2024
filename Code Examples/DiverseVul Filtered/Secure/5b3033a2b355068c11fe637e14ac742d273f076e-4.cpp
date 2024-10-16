FLAC__bool read_subframe_verbatim_(FLAC__StreamDecoder *decoder, unsigned channel, unsigned bps, FLAC__bool do_full_decode)
{
	FLAC__Subframe_Verbatim *subframe = &decoder->private_->frame.subframes[channel].data.verbatim;
	FLAC__int32 x, *residual = decoder->private_->residual[channel];
	unsigned i;

	decoder->private_->frame.subframes[channel].type = FLAC__SUBFRAME_TYPE_VERBATIM;

	subframe->data = residual;

	for(i = 0; i < decoder->private_->frame.header.blocksize; i++) {
		if(!FLAC__bitreader_read_raw_int32(decoder->private_->input, &x, bps))
			return false; /* read_callback_ sets the state for us */
		residual[i] = x;
	}

	/* decode the subframe */
	if(do_full_decode)
		memcpy(decoder->private_->output[channel], subframe->data, sizeof(FLAC__int32) * decoder->private_->frame.header.blocksize);

	return true;
}
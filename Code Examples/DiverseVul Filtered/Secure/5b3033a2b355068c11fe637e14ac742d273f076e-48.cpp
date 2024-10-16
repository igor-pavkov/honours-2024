FLAC__bool skip_id3v2_tag_(FLAC__StreamDecoder *decoder)
{
	FLAC__uint32 x;
	unsigned i, skip;

	/* skip the version and flags bytes */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 24))
		return false; /* read_callback_ sets the state for us */
	/* get the size (in bytes) to skip */
	skip = 0;
	for(i = 0; i < 4; i++) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
			return false; /* read_callback_ sets the state for us */
		skip <<= 7;
		skip |= (x & 0x7f);
	}
	/* skip the rest of the tag */
	if(!FLAC__bitreader_skip_byte_block_aligned_no_crc(decoder->private_->input, skip))
		return false; /* read_callback_ sets the state for us */
	return true;
}
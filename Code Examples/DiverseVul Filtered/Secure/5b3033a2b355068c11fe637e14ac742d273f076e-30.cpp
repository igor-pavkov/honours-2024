FLAC__bool read_frame_header_(FLAC__StreamDecoder *decoder)
{
	FLAC__uint32 x;
	FLAC__uint64 xx;
	unsigned i, blocksize_hint = 0, sample_rate_hint = 0;
	FLAC__byte crc8, raw_header[16]; /* MAGIC NUMBER based on the maximum frame header size, including CRC */
	unsigned raw_header_len;
	FLAC__bool is_unparseable = false;

	FLAC__ASSERT(FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input));

	/* init the raw header with the saved bits from synchronization */
	raw_header[0] = decoder->private_->header_warmup[0];
	raw_header[1] = decoder->private_->header_warmup[1];
	raw_header_len = 2;

	/* check to make sure that reserved bit is 0 */
	if(raw_header[1] & 0x02) /* MAGIC NUMBER */
		is_unparseable = true;

	/*
	 * Note that along the way as we read the header, we look for a sync
	 * code inside.  If we find one it would indicate that our original
	 * sync was bad since there cannot be a sync code in a valid header.
	 *
	 * Three kinds of things can go wrong when reading the frame header:
	 *  1) We may have sync'ed incorrectly and not landed on a frame header.
	 *     If we don't find a sync code, it can end up looking like we read
	 *     a valid but unparseable header, until getting to the frame header
	 *     CRC.  Even then we could get a false positive on the CRC.
	 *  2) We may have sync'ed correctly but on an unparseable frame (from a
	 *     future encoder).
	 *  3) We may be on a damaged frame which appears valid but unparseable.
	 *
	 * For all these reasons, we try and read a complete frame header as
	 * long as it seems valid, even if unparseable, up until the frame
	 * header CRC.
	 */

	/*
	 * read in the raw header as bytes so we can CRC it, and parse it on the way
	 */
	for(i = 0; i < 2; i++) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
			return false; /* read_callback_ sets the state for us */
		if(x == 0xff) { /* MAGIC NUMBER for the first 8 frame sync bits */
			/* if we get here it means our original sync was erroneous since the sync code cannot appear in the header */
			decoder->private_->lookahead = (FLAC__byte)x;
			decoder->private_->cached = true;
			send_error_to_client_(decoder, FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER);
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		}
		raw_header[raw_header_len++] = (FLAC__byte)x;
	}

	switch(x = raw_header[2] >> 4) {
		case 0:
			is_unparseable = true;
			break;
		case 1:
			decoder->private_->frame.header.blocksize = 192;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			decoder->private_->frame.header.blocksize = 576 << (x-2);
			break;
		case 6:
		case 7:
			blocksize_hint = x;
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			decoder->private_->frame.header.blocksize = 256 << (x-8);
			break;
		default:
			FLAC__ASSERT(0);
			break;
	}

	switch(x = raw_header[2] & 0x0f) {
		case 0:
			if(decoder->private_->has_stream_info)
				decoder->private_->frame.header.sample_rate = decoder->private_->stream_info.data.stream_info.sample_rate;
			else
				is_unparseable = true;
			break;
		case 1:
			decoder->private_->frame.header.sample_rate = 88200;
			break;
		case 2:
			decoder->private_->frame.header.sample_rate = 176400;
			break;
		case 3:
			decoder->private_->frame.header.sample_rate = 192000;
			break;
		case 4:
			decoder->private_->frame.header.sample_rate = 8000;
			break;
		case 5:
			decoder->private_->frame.header.sample_rate = 16000;
			break;
		case 6:
			decoder->private_->frame.header.sample_rate = 22050;
			break;
		case 7:
			decoder->private_->frame.header.sample_rate = 24000;
			break;
		case 8:
			decoder->private_->frame.header.sample_rate = 32000;
			break;
		case 9:
			decoder->private_->frame.header.sample_rate = 44100;
			break;
		case 10:
			decoder->private_->frame.header.sample_rate = 48000;
			break;
		case 11:
			decoder->private_->frame.header.sample_rate = 96000;
			break;
		case 12:
		case 13:
		case 14:
			sample_rate_hint = x;
			break;
		case 15:
			send_error_to_client_(decoder, FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER);
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		default:
			FLAC__ASSERT(0);
	}

	x = (unsigned)(raw_header[3] >> 4);
	if(x & 8) {
		decoder->private_->frame.header.channels = 2;
		switch(x & 7) {
			case 0:
				decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_LEFT_SIDE;
				break;
			case 1:
				decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_RIGHT_SIDE;
				break;
			case 2:
				decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_MID_SIDE;
				break;
			default:
				is_unparseable = true;
				break;
		}
	}
	else {
		decoder->private_->frame.header.channels = (unsigned)x + 1;
		decoder->private_->frame.header.channel_assignment = FLAC__CHANNEL_ASSIGNMENT_INDEPENDENT;
	}

	switch(x = (unsigned)(raw_header[3] & 0x0e) >> 1) {
		case 0:
			if(decoder->private_->has_stream_info)
				decoder->private_->frame.header.bits_per_sample = decoder->private_->stream_info.data.stream_info.bits_per_sample;
			else
				is_unparseable = true;
			break;
		case 1:
			decoder->private_->frame.header.bits_per_sample = 8;
			break;
		case 2:
			decoder->private_->frame.header.bits_per_sample = 12;
			break;
		case 4:
			decoder->private_->frame.header.bits_per_sample = 16;
			break;
		case 5:
			decoder->private_->frame.header.bits_per_sample = 20;
			break;
		case 6:
			decoder->private_->frame.header.bits_per_sample = 24;
			break;
		case 3:
		case 7:
			is_unparseable = true;
			break;
		default:
			FLAC__ASSERT(0);
			break;
	}

	/* check to make sure that reserved bit is 0 */
	if(raw_header[3] & 0x01) /* MAGIC NUMBER */
		is_unparseable = true;

	/* read the frame's starting sample number (or frame number as the case may be) */
	if(
		raw_header[1] & 0x01 ||
		/*@@@ this clause is a concession to the old way of doing variable blocksize; the only known implementation is flake and can probably be removed without inconveniencing anyone */
		(decoder->private_->has_stream_info && decoder->private_->stream_info.data.stream_info.min_blocksize != decoder->private_->stream_info.data.stream_info.max_blocksize)
	) { /* variable blocksize */
		if(!FLAC__bitreader_read_utf8_uint64(decoder->private_->input, &xx, raw_header, &raw_header_len))
			return false; /* read_callback_ sets the state for us */
		if(xx == FLAC__U64L(0xffffffffffffffff)) { /* i.e. non-UTF8 code... */
			decoder->private_->lookahead = raw_header[raw_header_len-1]; /* back up as much as we can */
			decoder->private_->cached = true;
			send_error_to_client_(decoder, FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER);
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		}
		decoder->private_->frame.header.number_type = FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER;
		decoder->private_->frame.header.number.sample_number = xx;
	}
	else { /* fixed blocksize */
		if(!FLAC__bitreader_read_utf8_uint32(decoder->private_->input, &x, raw_header, &raw_header_len))
			return false; /* read_callback_ sets the state for us */
		if(x == 0xffffffff) { /* i.e. non-UTF8 code... */
			decoder->private_->lookahead = raw_header[raw_header_len-1]; /* back up as much as we can */
			decoder->private_->cached = true;
			send_error_to_client_(decoder, FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER);
			decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
			return true;
		}
		decoder->private_->frame.header.number_type = FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER;
		decoder->private_->frame.header.number.frame_number = x;
	}

	if(blocksize_hint) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
			return false; /* read_callback_ sets the state for us */
		raw_header[raw_header_len++] = (FLAC__byte)x;
		if(blocksize_hint == 7) {
			FLAC__uint32 _x;
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &_x, 8))
				return false; /* read_callback_ sets the state for us */
			raw_header[raw_header_len++] = (FLAC__byte)_x;
			x = (x << 8) | _x;
		}
		decoder->private_->frame.header.blocksize = x+1;
	}

	if(sample_rate_hint) {
		if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
			return false; /* read_callback_ sets the state for us */
		raw_header[raw_header_len++] = (FLAC__byte)x;
		if(sample_rate_hint != 12) {
			FLAC__uint32 _x;
			if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &_x, 8))
				return false; /* read_callback_ sets the state for us */
			raw_header[raw_header_len++] = (FLAC__byte)_x;
			x = (x << 8) | _x;
		}
		if(sample_rate_hint == 12)
			decoder->private_->frame.header.sample_rate = x*1000;
		else if(sample_rate_hint == 13)
			decoder->private_->frame.header.sample_rate = x;
		else
			decoder->private_->frame.header.sample_rate = x*10;
	}

	/* read the CRC-8 byte */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, 8))
		return false; /* read_callback_ sets the state for us */
	crc8 = (FLAC__byte)x;

	if(FLAC__crc8(raw_header, raw_header_len) != crc8) {
		send_error_to_client_(decoder, FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER);
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}

	/* calculate the sample number from the frame number if needed */
	decoder->private_->next_fixed_block_size = 0;
	if(decoder->private_->frame.header.number_type == FLAC__FRAME_NUMBER_TYPE_FRAME_NUMBER) {
		x = decoder->private_->frame.header.number.frame_number;
		decoder->private_->frame.header.number_type = FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER;
		if(decoder->private_->fixed_block_size)
			decoder->private_->frame.header.number.sample_number = (FLAC__uint64)decoder->private_->fixed_block_size * (FLAC__uint64)x;
		else if(decoder->private_->has_stream_info) {
			if(decoder->private_->stream_info.data.stream_info.min_blocksize == decoder->private_->stream_info.data.stream_info.max_blocksize) {
				decoder->private_->frame.header.number.sample_number = (FLAC__uint64)decoder->private_->stream_info.data.stream_info.min_blocksize * (FLAC__uint64)x;
				decoder->private_->next_fixed_block_size = decoder->private_->stream_info.data.stream_info.max_blocksize;
			}
			else
				is_unparseable = true;
		}
		else if(x == 0) {
			decoder->private_->frame.header.number.sample_number = 0;
			decoder->private_->next_fixed_block_size = decoder->private_->frame.header.blocksize;
		}
		else {
			/* can only get here if the stream has invalid frame numbering and no STREAMINFO, so assume it's not the last (possibly short) frame */
			decoder->private_->frame.header.number.sample_number = (FLAC__uint64)decoder->private_->frame.header.blocksize * (FLAC__uint64)x;
		}
	}

	if(is_unparseable) {
		send_error_to_client_(decoder, FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM);
		decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;
		return true;
	}

	return true;
}
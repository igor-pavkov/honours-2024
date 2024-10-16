FLAC__bool read_metadata_picture_(FLAC__StreamDecoder *decoder, FLAC__StreamMetadata_Picture *obj)
{
	FLAC__uint32 x;

	FLAC__ASSERT(FLAC__bitreader_is_consumed_byte_aligned(decoder->private_->input));

	/* read type */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, FLAC__STREAM_METADATA_PICTURE_TYPE_LEN))
		return false; /* read_callback_ sets the state for us */
	obj->type = x;

	/* read MIME type */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, FLAC__STREAM_METADATA_PICTURE_MIME_TYPE_LENGTH_LEN))
		return false; /* read_callback_ sets the state for us */
	if(0 == (obj->mime_type = safe_malloc_add_2op_(x, /*+*/1))) {
		decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}
	if(x > 0) {
		if(!FLAC__bitreader_read_byte_block_aligned_no_crc(decoder->private_->input, (FLAC__byte*)obj->mime_type, x))
			return false; /* read_callback_ sets the state for us */
	}
	obj->mime_type[x] = '\0';

	/* read description */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &x, FLAC__STREAM_METADATA_PICTURE_DESCRIPTION_LENGTH_LEN))
		return false; /* read_callback_ sets the state for us */
	if(0 == (obj->description = safe_malloc_add_2op_(x, /*+*/1))) {
		decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}
	if(x > 0) {
		if(!FLAC__bitreader_read_byte_block_aligned_no_crc(decoder->private_->input, obj->description, x))
			return false; /* read_callback_ sets the state for us */
	}
	obj->description[x] = '\0';

	/* read width */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &obj->width, FLAC__STREAM_METADATA_PICTURE_WIDTH_LEN))
		return false; /* read_callback_ sets the state for us */

	/* read height */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &obj->height, FLAC__STREAM_METADATA_PICTURE_HEIGHT_LEN))
		return false; /* read_callback_ sets the state for us */

	/* read depth */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &obj->depth, FLAC__STREAM_METADATA_PICTURE_DEPTH_LEN))
		return false; /* read_callback_ sets the state for us */

	/* read colors */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &obj->colors, FLAC__STREAM_METADATA_PICTURE_COLORS_LEN))
		return false; /* read_callback_ sets the state for us */

	/* read data */
	if(!FLAC__bitreader_read_raw_uint32(decoder->private_->input, &(obj->data_length), FLAC__STREAM_METADATA_PICTURE_DATA_LENGTH_LEN))
		return false; /* read_callback_ sets the state for us */
	if(0 == (obj->data = safe_malloc_(obj->data_length))) {
		decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}
	if(obj->data_length > 0) {
		if(!FLAC__bitreader_read_byte_block_aligned_no_crc(decoder->private_->input, obj->data, obj->data_length))
			return false; /* read_callback_ sets the state for us */
	}

	return true;
}
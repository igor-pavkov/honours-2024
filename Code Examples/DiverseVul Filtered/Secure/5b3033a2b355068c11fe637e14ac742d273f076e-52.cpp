FLAC_API FLAC__bool FLAC__stream_decoder_seek_absolute(FLAC__StreamDecoder *decoder, FLAC__uint64 sample)
{
	FLAC__uint64 length;

	FLAC__ASSERT(0 != decoder);

	if(
		decoder->protected_->state != FLAC__STREAM_DECODER_SEARCH_FOR_METADATA &&
		decoder->protected_->state != FLAC__STREAM_DECODER_READ_METADATA &&
		decoder->protected_->state != FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC &&
		decoder->protected_->state != FLAC__STREAM_DECODER_READ_FRAME &&
		decoder->protected_->state != FLAC__STREAM_DECODER_END_OF_STREAM
	)
		return false;

	if(0 == decoder->private_->seek_callback)
		return false;

	FLAC__ASSERT(decoder->private_->seek_callback);
	FLAC__ASSERT(decoder->private_->tell_callback);
	FLAC__ASSERT(decoder->private_->length_callback);
	FLAC__ASSERT(decoder->private_->eof_callback);

	if(FLAC__stream_decoder_get_total_samples(decoder) > 0 && sample >= FLAC__stream_decoder_get_total_samples(decoder))
		return false;

	decoder->private_->is_seeking = true;

	/* turn off md5 checking if a seek is attempted */
	decoder->private_->do_md5_checking = false;

	/* get the file length (currently our algorithm needs to know the length so it's also an error to get FLAC__STREAM_DECODER_LENGTH_STATUS_UNSUPPORTED) */
	if(decoder->private_->length_callback(decoder, &length, decoder->private_->client_data) != FLAC__STREAM_DECODER_LENGTH_STATUS_OK) {
		decoder->private_->is_seeking = false;
		return false;
	}

	/* if we haven't finished processing the metadata yet, do that so we have the STREAMINFO, SEEK_TABLE, and first_frame_offset */
	if(
		decoder->protected_->state == FLAC__STREAM_DECODER_SEARCH_FOR_METADATA ||
		decoder->protected_->state == FLAC__STREAM_DECODER_READ_METADATA
	) {
		if(!FLAC__stream_decoder_process_until_end_of_metadata(decoder)) {
			/* above call sets the state for us */
			decoder->private_->is_seeking = false;
			return false;
		}
		/* check this again in case we didn't know total_samples the first time */
		if(FLAC__stream_decoder_get_total_samples(decoder) > 0 && sample >= FLAC__stream_decoder_get_total_samples(decoder)) {
			decoder->private_->is_seeking = false;
			return false;
		}
	}

	{
		const FLAC__bool ok =
#if FLAC__HAS_OGG
			decoder->private_->is_ogg?
			seek_to_absolute_sample_ogg_(decoder, length, sample) :
#endif
			seek_to_absolute_sample_(decoder, length, sample)
		;
		decoder->private_->is_seeking = false;
		return ok;
	}
}
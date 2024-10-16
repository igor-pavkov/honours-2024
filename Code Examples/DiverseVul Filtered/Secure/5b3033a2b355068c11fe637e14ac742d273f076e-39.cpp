FLAC_API FLAC__bool FLAC__stream_decoder_flush(FLAC__StreamDecoder *decoder)
{
	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != decoder->protected_);

	decoder->private_->samples_decoded = 0;
	decoder->private_->do_md5_checking = false;

#if FLAC__HAS_OGG
	if(decoder->private_->is_ogg)
		FLAC__ogg_decoder_aspect_flush(&decoder->protected_->ogg_decoder_aspect);
#endif

	if(!FLAC__bitreader_clear(decoder->private_->input)) {
		decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
		return false;
	}
	decoder->protected_->state = FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC;

	return true;
}
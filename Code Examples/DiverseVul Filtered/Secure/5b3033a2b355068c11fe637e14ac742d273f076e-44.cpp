void set_defaults_(FLAC__StreamDecoder *decoder)
{
#if FLAC__HAS_OGG
	decoder->private_->is_ogg = false;
#endif
	decoder->private_->read_callback = 0;
	decoder->private_->seek_callback = 0;
	decoder->private_->tell_callback = 0;
	decoder->private_->length_callback = 0;
	decoder->private_->eof_callback = 0;
	decoder->private_->write_callback = 0;
	decoder->private_->metadata_callback = 0;
	decoder->private_->error_callback = 0;
	decoder->private_->client_data = 0;

	memset(decoder->private_->metadata_filter, 0, sizeof(decoder->private_->metadata_filter));
	decoder->private_->metadata_filter[FLAC__METADATA_TYPE_STREAMINFO] = true;
	decoder->private_->metadata_filter_ids_count = 0;

	decoder->protected_->md5_checking = false;

#if FLAC__HAS_OGG
	FLAC__ogg_decoder_aspect_set_defaults(&decoder->protected_->ogg_decoder_aspect);
#endif
}
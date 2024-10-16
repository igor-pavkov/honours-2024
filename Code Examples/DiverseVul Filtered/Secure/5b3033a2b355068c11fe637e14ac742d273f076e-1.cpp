FLAC_API FLAC__bool FLAC__stream_decoder_finish(FLAC__StreamDecoder *decoder)
{
	FLAC__bool md5_failed = false;
	unsigned i;

	FLAC__ASSERT(0 != decoder);
	FLAC__ASSERT(0 != decoder->private_);
	FLAC__ASSERT(0 != decoder->protected_);

	if(decoder->protected_->state == FLAC__STREAM_DECODER_UNINITIALIZED)
		return true;

	/* see the comment in FLAC__stream_decoder_reset() as to why we
	 * always call FLAC__MD5Final()
	 */
	FLAC__MD5Final(decoder->private_->computed_md5sum, &decoder->private_->md5context);

	if(decoder->private_->has_seek_table && 0 != decoder->private_->seek_table.data.seek_table.points) {
		free(decoder->private_->seek_table.data.seek_table.points);
		decoder->private_->seek_table.data.seek_table.points = 0;
		decoder->private_->has_seek_table = false;
	}
	FLAC__bitreader_free(decoder->private_->input);
	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		/* WATCHOUT:
		 * FLAC__lpc_restore_signal_asm_ia32_mmx() requires that the
		 * output arrays have a buffer of up to 3 zeroes in front
		 * (at negative indices) for alignment purposes; we use 4
		 * to keep the data well-aligned.
		 */
		if(0 != decoder->private_->output[i]) {
			free(decoder->private_->output[i]-4);
			decoder->private_->output[i] = 0;
		}
		if(0 != decoder->private_->residual_unaligned[i]) {
			free(decoder->private_->residual_unaligned[i]);
			decoder->private_->residual_unaligned[i] = decoder->private_->residual[i] = 0;
		}
	}
	decoder->private_->output_capacity = 0;
	decoder->private_->output_channels = 0;

#if FLAC__HAS_OGG
	if(decoder->private_->is_ogg)
		FLAC__ogg_decoder_aspect_finish(&decoder->protected_->ogg_decoder_aspect);
#endif

	if(0 != decoder->private_->file) {
		if(decoder->private_->file != stdin)
			fclose(decoder->private_->file);
		decoder->private_->file = 0;
	}

	if(decoder->private_->do_md5_checking) {
		if(memcmp(decoder->private_->stream_info.data.stream_info.md5sum, decoder->private_->computed_md5sum, 16))
			md5_failed = true;
	}
	decoder->private_->is_seeking = false;

	set_defaults_(decoder);

	decoder->protected_->state = FLAC__STREAM_DECODER_UNINITIALIZED;

	return !md5_failed;
}
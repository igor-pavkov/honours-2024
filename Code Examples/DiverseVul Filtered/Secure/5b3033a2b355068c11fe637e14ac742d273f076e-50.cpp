FLAC__bool allocate_output_(FLAC__StreamDecoder *decoder, unsigned size, unsigned channels)
{
	unsigned i;
	FLAC__int32 *tmp;

	if(size <= decoder->private_->output_capacity && channels <= decoder->private_->output_channels)
		return true;

	/* simply using realloc() is not practical because the number of channels may change mid-stream */

	for(i = 0; i < FLAC__MAX_CHANNELS; i++) {
		if(0 != decoder->private_->output[i]) {
			free(decoder->private_->output[i]-4);
			decoder->private_->output[i] = 0;
		}
		if(0 != decoder->private_->residual_unaligned[i]) {
			free(decoder->private_->residual_unaligned[i]);
			decoder->private_->residual_unaligned[i] = decoder->private_->residual[i] = 0;
		}
	}

	for(i = 0; i < channels; i++) {
		/* WATCHOUT:
		 * FLAC__lpc_restore_signal_asm_ia32_mmx() requires that the
		 * output arrays have a buffer of up to 3 zeroes in front
		 * (at negative indices) for alignment purposes; we use 4
		 * to keep the data well-aligned.
		 */
		tmp = safe_malloc_muladd2_(sizeof(FLAC__int32), /*times (*/size, /*+*/4/*)*/);
		if(tmp == 0) {
			decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
			return false;
		}
		memset(tmp, 0, sizeof(FLAC__int32)*4);
		decoder->private_->output[i] = tmp + 4;

		if(!FLAC__memory_alloc_aligned_int32_array(size, &decoder->private_->residual_unaligned[i], &decoder->private_->residual[i])) {
			decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
			return false;
		}
	}

	decoder->private_->output_capacity = size;
	decoder->private_->output_channels = channels;

	return true;
}
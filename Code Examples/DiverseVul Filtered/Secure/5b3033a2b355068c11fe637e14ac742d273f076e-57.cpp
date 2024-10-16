static FLAC__StreamDecoderInitStatus init_stream_internal_(
	FLAC__StreamDecoder *decoder,
	FLAC__StreamDecoderReadCallback read_callback,
	FLAC__StreamDecoderSeekCallback seek_callback,
	FLAC__StreamDecoderTellCallback tell_callback,
	FLAC__StreamDecoderLengthCallback length_callback,
	FLAC__StreamDecoderEofCallback eof_callback,
	FLAC__StreamDecoderWriteCallback write_callback,
	FLAC__StreamDecoderMetadataCallback metadata_callback,
	FLAC__StreamDecoderErrorCallback error_callback,
	void *client_data,
	FLAC__bool is_ogg
)
{
	FLAC__ASSERT(0 != decoder);

	if(decoder->protected_->state != FLAC__STREAM_DECODER_UNINITIALIZED)
		return FLAC__STREAM_DECODER_INIT_STATUS_ALREADY_INITIALIZED;

#if !FLAC__HAS_OGG
	if(is_ogg)
		return FLAC__STREAM_DECODER_INIT_STATUS_UNSUPPORTED_CONTAINER;
#endif

	if(
		0 == read_callback ||
		0 == write_callback ||
		0 == error_callback ||
		(seek_callback && (0 == tell_callback || 0 == length_callback || 0 == eof_callback))
	)
		return FLAC__STREAM_DECODER_INIT_STATUS_INVALID_CALLBACKS;

#if FLAC__HAS_OGG
	decoder->private_->is_ogg = is_ogg;
	if(is_ogg && !FLAC__ogg_decoder_aspect_init(&decoder->protected_->ogg_decoder_aspect))
		return decoder->protected_->initstate = FLAC__STREAM_DECODER_INIT_STATUS_ERROR_OPENING_FILE;
#endif

	/*
	 * get the CPU info and set the function pointers
	 */
	FLAC__cpu_info(&decoder->private_->cpuinfo);
	/* first default to the non-asm routines */
	decoder->private_->local_lpc_restore_signal = FLAC__lpc_restore_signal;
	decoder->private_->local_lpc_restore_signal_64bit = FLAC__lpc_restore_signal_wide;
	decoder->private_->local_lpc_restore_signal_16bit = FLAC__lpc_restore_signal;
	/* now override with asm where appropriate */
#ifndef FLAC__NO_ASM
	if(decoder->private_->cpuinfo.use_asm) {
#ifdef FLAC__CPU_IA32
		FLAC__ASSERT(decoder->private_->cpuinfo.type == FLAC__CPUINFO_TYPE_IA32);
#ifdef FLAC__HAS_NASM
		decoder->private_->local_lpc_restore_signal_64bit = FLAC__lpc_restore_signal_wide_asm_ia32; /* OPT_IA32: was really necessary for GCC < 4.9 */
		if(decoder->private_->cpuinfo.ia32.mmx) {
			decoder->private_->local_lpc_restore_signal = FLAC__lpc_restore_signal_asm_ia32;
			decoder->private_->local_lpc_restore_signal_16bit = FLAC__lpc_restore_signal_asm_ia32_mmx;
		}
		else {
			decoder->private_->local_lpc_restore_signal = FLAC__lpc_restore_signal_asm_ia32;
			decoder->private_->local_lpc_restore_signal_16bit = FLAC__lpc_restore_signal_asm_ia32;
		}
#endif
#ifdef FLAC__HAS_X86INTRIN
# if defined FLAC__SSE2_SUPPORTED && !defined FLAC__HAS_NASM /* OPT_SSE: not better than MMX asm */
		if(decoder->private_->cpuinfo.ia32.sse2) {
			decoder->private_->local_lpc_restore_signal_16bit = FLAC__lpc_restore_signal_16_intrin_sse2;
		}
# endif
# if defined FLAC__SSE4_1_SUPPORTED
		if(decoder->private_->cpuinfo.ia32.sse41) {
			decoder->private_->local_lpc_restore_signal_64bit = FLAC__lpc_restore_signal_wide_intrin_sse41;
		}
# endif
#endif
#elif defined FLAC__CPU_X86_64
		FLAC__ASSERT(decoder->private_->cpuinfo.type == FLAC__CPUINFO_TYPE_X86_64);
		/* No useful SSE optimizations yet */
#endif
	}
#endif

	/* from here on, errors are fatal */

	if(!FLAC__bitreader_init(decoder->private_->input, read_callback_, decoder)) {
		decoder->protected_->state = FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR;
		return FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR;
	}

	decoder->private_->read_callback = read_callback;
	decoder->private_->seek_callback = seek_callback;
	decoder->private_->tell_callback = tell_callback;
	decoder->private_->length_callback = length_callback;
	decoder->private_->eof_callback = eof_callback;
	decoder->private_->write_callback = write_callback;
	decoder->private_->metadata_callback = metadata_callback;
	decoder->private_->error_callback = error_callback;
	decoder->private_->client_data = client_data;
	decoder->private_->fixed_block_size = decoder->private_->next_fixed_block_size = 0;
	decoder->private_->samples_decoded = 0;
	decoder->private_->has_stream_info = false;
	decoder->private_->cached = false;

	decoder->private_->do_md5_checking = decoder->protected_->md5_checking;
	decoder->private_->is_seeking = false;

	decoder->private_->internal_reset_hack = true; /* so the following reset does not try to rewind the input */
	if(!FLAC__stream_decoder_reset(decoder)) {
		/* above call sets the state for us */
		return FLAC__STREAM_DECODER_INIT_STATUS_MEMORY_ALLOCATION_ERROR;
	}

	return FLAC__STREAM_DECODER_INIT_STATUS_OK;
}
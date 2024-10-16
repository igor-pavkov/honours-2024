AudioOutputSpeech::~AudioOutputSpeech() {
#ifdef USE_OPUS
	if (opusState)
		opus_decoder_destroy(opusState);
#endif
	if (cdDecoder) {
		cCodec->celt_decoder_destroy(cdDecoder);
	} else if (dsSpeex) {
		speex_bits_destroy(&sbBits);
		speex_decoder_destroy(dsSpeex);
	}

	if (srs)
		speex_resampler_destroy(srs);

	jitter_buffer_destroy(jbJitter);

	delete [] fFadeIn;
	delete [] fFadeOut;
	delete [] fResamplerBuffer;
}
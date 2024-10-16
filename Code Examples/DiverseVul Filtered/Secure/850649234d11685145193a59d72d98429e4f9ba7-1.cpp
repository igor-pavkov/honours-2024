AudioOutputSpeech::AudioOutputSpeech(ClientUser *user, unsigned int freq, MessageHandler::UDPMessageType type) : AudioOutputUser(user->qsName) {
	int err;
	p = user;
	umtType = type;
	iMixerFreq = freq;

	cCodec = NULL;
	cdDecoder = NULL;
	dsSpeex = NULL;
	opusState = NULL;

	bHasTerminator = false;
	bStereo = false;

	iSampleRate = SAMPLE_RATE;
	iFrameSize = iSampleRate / 100;
	iAudioBufferSize = iFrameSize;

	if (umtType == MessageHandler::UDPVoiceOpus) {
#ifdef USE_OPUS
		iAudioBufferSize *= 12;
		opusState = opus_decoder_create(iSampleRate, bStereo ? 2 : 1, NULL);
#endif
	} else if (umtType == MessageHandler::UDPVoiceSpeex) {
		speex_bits_init(&sbBits);

		dsSpeex = speex_decoder_init(speex_lib_get_mode(SPEEX_MODEID_UWB));
		int iArg=1;
		speex_decoder_ctl(dsSpeex, SPEEX_SET_ENH, &iArg);
		speex_decoder_ctl(dsSpeex, SPEEX_GET_FRAME_SIZE, &iFrameSize);
		speex_decoder_ctl(dsSpeex, SPEEX_GET_SAMPLING_RATE, &iSampleRate);
		iAudioBufferSize = iFrameSize;
	}

	iOutputSize = static_cast<unsigned int>(ceilf(static_cast<float>(iAudioBufferSize * iMixerFreq) / static_cast<float>(iSampleRate)));
	if (bStereo) {
		iAudioBufferSize *= 2;
		iOutputSize *= 2;
	}

	srs = NULL;
	fResamplerBuffer = NULL;
	if (iMixerFreq != iSampleRate) {
		srs = speex_resampler_init(bStereo ? 2 : 1, iSampleRate, iMixerFreq, 3, &err);
		fResamplerBuffer = new float[iAudioBufferSize];
	}

	iBufferOffset = iBufferFilled = iLastConsume = 0;
	bLastAlive = true;

	iMissCount = 0;
	iMissedFrames = 0;

	ucFlags = 0xFF;

	jbJitter = jitter_buffer_init(iFrameSize);
	int margin = g.s.iJitterBufferSize * iFrameSize;
	jitter_buffer_ctl(jbJitter, JITTER_BUFFER_SET_MARGIN, &margin);

	fFadeIn = new float[iFrameSize];
	fFadeOut = new float[iFrameSize];

	float mul = static_cast<float>(M_PI / (2.0 * static_cast<double>(iFrameSize)));
	for (unsigned int i=0;i<iFrameSize;++i)
		fFadeIn[i] = fFadeOut[iFrameSize-i-1] = sinf(static_cast<float>(i) * mul);
}
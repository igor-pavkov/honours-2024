int AVI_can_read_audio(avi_t *AVI)
{
	if(AVI->mode==AVI_MODE_WRITE) {
		return -1;
	}
	if(!AVI->video_index)         {
		return -1;
	}
	if(!AVI->track[AVI->aptr].audio_index)         {
		return -1;
	}

	// is it -1? the last ones got left out --tibit
	//if (AVI->track[AVI->aptr].audio_posc>=AVI->track[AVI->aptr].audio_chunks-1) {
	if (AVI->track[AVI->aptr].audio_posc>=AVI->track[AVI->aptr].audio_chunks) {
		return 0;
	}

	if (AVI->video_pos >= AVI->video_frames) return 1;

	if (AVI->track[AVI->aptr].audio_index[AVI->track[AVI->aptr].audio_posc].pos < AVI->video_index[AVI->video_pos].pos) return 1;
	else return 0;
}
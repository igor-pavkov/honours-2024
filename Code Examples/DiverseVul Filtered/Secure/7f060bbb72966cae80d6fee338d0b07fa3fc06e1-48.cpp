int AVI_set_audio_track(avi_t *AVI, u32 track)
{

	if (track + 1 > AVI->anum) return(-1);

	//this info is not written to file anyway
	AVI->aptr=track;
	return 0;
}
GF_Err GetPrevMediaTime(GF_TrackBox *trak, u64 movieTime, u64 *OutMovieTime)
{
	u32 i;
	u64 time;
	GF_EdtsEntry *ent;

	*OutMovieTime = 0;
	if (! trak->editBox || !trak->editBox->editList) return GF_BAD_PARAM;

	time = 0;
	ent = NULL;
	i=0;
	while ((ent = (GF_EdtsEntry *)gf_list_enum(trak->editBox->editList->entryList, &i))) {
		if (ent->mediaTime == -1) {
			if ( (time + ent->segmentDuration) * trak->Media->mediaHeader->timeScale >= movieTime * trak->moov->mvhd->timeScale) {
				*OutMovieTime = time * trak->Media->mediaHeader->timeScale / trak->moov->mvhd->timeScale;
				return GF_OK;
			}
			continue;
		}
		/*get the first entry whose end is greater than or equal to the desired time*/
		time += ent->segmentDuration;
		if ( time * trak->Media->mediaHeader->timeScale >= movieTime * trak->moov->mvhd->timeScale) {
			*OutMovieTime = time * trak->Media->mediaHeader->timeScale / trak->moov->mvhd->timeScale;
			return GF_OK;
		}
	}
	*OutMovieTime = 0;
	return GF_OK;
}
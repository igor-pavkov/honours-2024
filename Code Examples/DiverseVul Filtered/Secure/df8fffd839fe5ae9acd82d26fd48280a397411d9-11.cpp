GF_Err SetTrackDuration(GF_TrackBox *trak)
{
	u64 trackDuration;
	u32 i;
	GF_Err e;

	//the total duration is the media duration: adjust it in case...
	e = Media_SetDuration(trak);
	if (e) return e;

	//assert the timeScales are non-NULL
	if (!trak->moov->mvhd || !trak->moov->mvhd->timeScale || !trak->Media->mediaHeader->timeScale) return GF_ISOM_INVALID_FILE;
	trackDuration = (trak->Media->mediaHeader->duration * trak->moov->mvhd->timeScale) / trak->Media->mediaHeader->timeScale;

	//if we have an edit list, the duration is the sum of all the editList
	//entries' duration (always expressed in MovieTimeScale)
	if (trak->editBox && trak->editBox->editList) {
		GF_EdtsEntry *ent;
		GF_EditListBox *elst = trak->editBox->editList;
		trackDuration = 0;
		i=0;
		while ((ent = (GF_EdtsEntry*)gf_list_enum(elst->entryList, &i))) {
			trackDuration += ent->segmentDuration;
		}
	}
	if (!trackDuration) {
		trackDuration = (trak->Media->mediaHeader->duration * trak->moov->mvhd->timeScale) / trak->Media->mediaHeader->timeScale;
	}
	if (!trak->Header) {
		return GF_OK;
	}
	trak->Header->duration = trackDuration;
	if (!trak->moov->mov->keep_utc && !gf_sys_is_test_mode() )
		trak->Header->modificationTime = gf_isom_get_mp4time();
	return GF_OK;
}
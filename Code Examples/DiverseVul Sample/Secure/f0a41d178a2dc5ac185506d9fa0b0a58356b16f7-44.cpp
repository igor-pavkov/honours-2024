Bool gf_isom_get_edit_list_type(GF_ISOFile *the_file, u32 trackNumber, s64 *mediaOffset)
{
	GF_EdtsEntry *ent;
	GF_TrackBox *trak;
	u32 count;
	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_FALSE;
	*mediaOffset = 0;
	if (!trak->editBox || !trak->editBox->editList) return GF_FALSE;

	count = gf_list_count(trak->editBox->editList->entryList);
	ent = (GF_EdtsEntry*)gf_list_get(trak->editBox->editList->entryList, 0);
	if (!ent) return GF_TRUE;
	/*mediaRate>0, the track playback shall start at media time>0 -> mediaOffset is < 0 */
	if ((count==1) && (ent->mediaRate == 0x10000) && (ent->mediaTime>=0)) {
		*mediaOffset = - ent->mediaTime;
		return GF_FALSE;
	} else if (count==2) {
		/*mediaTime==-1, the track playback shall be empty for segmentDuration -> mediaOffset is > 0 */
		if ((ent->mediaRate == -0x10000) || (ent->mediaTime==-1)) {
			Double time = (Double) ent->segmentDuration;
			time /= trak->moov->mvhd->timeScale;
			time *= trak->Media->mediaHeader->timeScale;
			*mediaOffset = (s64) time;

			//check next entry, if we start from mediaOffset > 0 this may still result in a skip
			ent = (GF_EdtsEntry*)gf_list_get(trak->editBox->editList->entryList, 1);
			//next entry playback rate is not nominal, we need edit list handling
			if (ent->mediaRate != 0x10000)
				return GF_TRUE;

			if (ent->mediaTime > 0) {
				*mediaOffset -= ent->mediaTime;
			}
			return GF_FALSE;
		}
	}
	return GF_TRUE;
}
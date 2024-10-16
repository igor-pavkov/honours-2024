s32 gf_isom_get_composition_offset_shift(GF_ISOFile *file, u32 track)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(file, track);
	if (!trak) return 0;
	if (!trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->CompositionToDecode) return 0;
	return trak->Media->information->sampleTable->CompositionToDecode->compositionToDTSShift;
}
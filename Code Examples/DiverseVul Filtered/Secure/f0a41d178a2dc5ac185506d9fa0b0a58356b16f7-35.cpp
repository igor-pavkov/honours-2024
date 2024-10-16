u32 gf_isom_get_chunk_count(GF_ISOFile *movie, u32 trackNumber)
{
	GF_ChunkOffsetBox *stco;
	GF_TrackBox *trak;
	if (!movie || !movie->moov || !trackNumber) return 0;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !trak->Media->information || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->ChunkOffset ) return 0;

	stco = (GF_ChunkOffsetBox *) trak->Media->information->sampleTable->ChunkOffset;
	if (stco->type == GF_ISOM_BOX_TYPE_STCO)
		return stco->nb_entries;
	if (stco->type == GF_ISOM_BOX_TYPE_CO64)
		return ((GF_ChunkLargeOffsetBox *) stco)->nb_entries;
	return 0;
}
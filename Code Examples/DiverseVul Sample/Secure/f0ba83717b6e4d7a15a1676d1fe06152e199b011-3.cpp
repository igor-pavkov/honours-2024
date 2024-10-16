u32 gf_isom_find_od_id_for_track(GF_ISOFile *file, u32 track)
{
	u32 i, j, di, the_od_id;
	GF_TrackBox *od_tk;
	GF_TrackBox *tk = gf_isom_get_track_from_file(file, track);
	if (!tk) return 0;

	i=0;
	while ( (od_tk = (GF_TrackBox*)gf_list_enum(file->moov->trackList, &i))) {
		if (od_tk->Media->handler->handlerType != GF_ISOM_MEDIA_OD) continue;

		for (j=0; j<od_tk->Media->information->sampleTable->SampleSize->sampleCount; j++) {
			GF_ISOSample *samp = gf_isom_get_sample(file, i, j+1, &di);
			the_od_id = Media_FindOD_ID(od_tk->Media, samp, tk->Header->trackID);
			gf_isom_sample_del(&samp);
			if (the_od_id) return the_od_id;
		}
	}
	return 0;
}
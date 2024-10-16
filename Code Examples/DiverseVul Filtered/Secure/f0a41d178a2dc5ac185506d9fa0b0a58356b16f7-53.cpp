u32 gf_isom_get_next_alternate_group_id(GF_ISOFile *movie)
{
	u32 id = 0;
	u32 i=0;

	while (i< gf_isom_get_track_count(movie) ) {
		GF_TrackBox *trak = gf_isom_get_track_from_file(movie, i+1);
		if (trak->Header->alternate_group > id)
			id = trak->Header->alternate_group;
		i++;
	}
	return id+1;
}
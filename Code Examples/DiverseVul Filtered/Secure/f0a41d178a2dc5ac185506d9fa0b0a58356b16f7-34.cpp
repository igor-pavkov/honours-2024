const u8 *gf_isom_get_mpegh_compatible_profiles(GF_ISOFile *movie, u32 trackNumber, u32 sampleDescIndex, u32 *nb_compat_profiles)
{
	GF_SampleEntryBox *ent;
	GF_MHACompatibleProfilesBox *mhap;
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Media || !nb_compat_profiles) return NULL;
	*nb_compat_profiles = 0;
	ent = gf_list_get(trak->Media->information->sampleTable->SampleDescription->child_boxes, sampleDescIndex-1);
	if (!ent) return NULL;
	mhap = (GF_MHACompatibleProfilesBox *) gf_isom_box_find_child(ent->child_boxes, GF_ISOM_BOX_TYPE_MHAP);
	if (!mhap) return NULL;
	*nb_compat_profiles = mhap->num_profiles;
	return mhap->compat_profiles;
}
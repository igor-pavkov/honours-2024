GF_Err gf_isom_get_track_switch_group_count(GF_ISOFile *movie, u32 trackNumber, u32 *alternateGroupID, u32 *nb_groups)
{
	GF_UserDataMap *map;
	GF_TrackBox *trak;

	trak = gf_isom_get_track_from_file(movie, trackNumber);
	if (!trak || !trak->Header) return GF_BAD_PARAM;
	if (alternateGroupID) *alternateGroupID = trak->Header->alternate_group;
	if (nb_groups) *nb_groups = 0;
	if (!trak->udta || !nb_groups) return GF_OK;

	map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_TSEL, NULL);
	if (!map) return GF_OK;
	*nb_groups = gf_list_count(map->boxes);
	return GF_OK;
}
u32 gf_isom_get_payt_count(GF_ISOFile *the_file, u32 trackNumber)
{
	u32 i, count;
	GF_TrackBox *trak;
	GF_UserDataMap *map;
	GF_HintInfoBox *hinf;
	GF_PAYTBox *payt;

	trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return 0;

	if (!CheckHintFormat(trak, GF_ISOM_HINT_RTP)) return 0;
	map = udta_getEntry(trak->udta, GF_ISOM_BOX_TYPE_HINF, NULL);
	if (!map) return 0;
	if (gf_list_count(map->boxes) != 1) return 0;

	hinf = (GF_HintInfoBox *)gf_list_get(map->boxes, 0);
	count = 0;
	i = 0;
	while ((payt = (GF_PAYTBox*)gf_list_enum(hinf->child_boxes, &i))) {
		if (payt->type == GF_ISOM_BOX_TYPE_PAYT) count++;
	}
	return count;
}
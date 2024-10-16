Bool gf_isom_get_tile_info(GF_ISOFile *file, u32 trackNumber, u32 sample_description_index, u32 *default_sample_group_index, u32 *id, u32 *independent, Bool *full_picture, u32 *x, u32 *y, u32 *w, u32 *h)
{
	const u8 *data;
	u32 size;

	if (!gf_isom_get_sample_group_info(file, trackNumber, sample_description_index, GF_ISOM_SAMPLE_GROUP_TRIF, default_sample_group_index, &data, &size))
		return GF_FALSE;
	gf_isom_parse_trif_info(data, size, id, independent, full_picture, x, y, w, h);
	return GF_TRUE;
}
GF_Err gf_isom_get_track_layout_info(GF_ISOFile *movie, u32 trackNumber, u32 *width, u32 *height, s32 *translation_x, s32 *translation_y, s16 *layer)
{
	GF_TrackBox *tk = gf_isom_get_track_from_file(movie, trackNumber);
	if (!tk) return GF_BAD_PARAM;
	if (width) *width = tk->Header->width>>16;
	if (height) *height = tk->Header->height>>16;
	if (layer) *layer = tk->Header->layer;
	if (translation_x) *translation_x = tk->Header->matrix[6] >> 16;
	if (translation_y) *translation_y = tk->Header->matrix[7] >> 16;
	return GF_OK;
}
GF_Err strk_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SubTrackBox *ptr = (GF_SubTrackBox *)s;
	e = gf_isom_box_array_read(s, bs);
	if (e) return e;

	if (!ptr->info) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing SubTrackInformationBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	return GF_OK;
}
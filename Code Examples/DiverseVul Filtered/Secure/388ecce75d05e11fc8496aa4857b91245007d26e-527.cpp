GF_Err mdia_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u64 cookie = gf_bs_get_cookie(bs);
	cookie &= ~GF_ISOM_BS_COOKIE_VISUAL_TRACK;
	gf_bs_set_cookie(bs, cookie);
	e = gf_isom_box_array_read(s, bs, mdia_on_child_box);
	gf_bs_set_cookie(bs, cookie);

	if (e) return e;
	if (!((GF_MediaBox *)s)->information) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MediaInformationBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!((GF_MediaBox *)s)->handler) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing HandlerBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	if (!((GF_MediaBox *)s)->mediaHeader) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing MediaHeaderBox\n"));
		return GF_ISOM_INVALID_FILE;
	}
	return GF_OK;
}
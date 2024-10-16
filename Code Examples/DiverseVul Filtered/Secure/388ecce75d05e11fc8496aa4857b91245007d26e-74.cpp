GF_Err traf_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrackFragmentBox *ptr = (GF_TrackFragmentBox *)s;
	GF_Err e = gf_isom_box_array_read(s, bs, traf_on_child_box);
	if (e) return e;

	if (!ptr->tfhd) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing TrackFragmentHeaderBox \n"));
		return GF_ISOM_INVALID_FILE;
	}
	return GF_OK;
}
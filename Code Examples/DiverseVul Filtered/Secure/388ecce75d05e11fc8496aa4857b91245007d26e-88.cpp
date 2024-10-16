GF_Err mdat_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MediaDataBox *ptr = (GF_MediaDataBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	ptr->dataSize = s->size;
	ptr->bsOffset = gf_bs_get_position(bs);

	//then skip these bytes
	gf_bs_skip_bytes(bs, ptr->dataSize);
	return GF_OK;
}
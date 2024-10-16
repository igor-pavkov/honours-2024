GF_Err mdat_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MediaDataBox *ptr = (GF_MediaDataBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;

	ptr->dataSize = s->size;
	ptr->bsOffset = gf_bs_get_position(bs);

	//store idat for rewrite
	if (ptr->type==GF_ISOM_BOX_TYPE_IDAT) {
		ptr->data = gf_malloc(sizeof(u8) * (size_t)ptr->dataSize);
		if (!ptr->data) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->data, (u32) ptr->dataSize);
		ptr->size = 0;
		return GF_OK;
	}

	//then skip these bytes
	gf_bs_skip_bytes(bs, ptr->dataSize);
	return GF_OK;
}
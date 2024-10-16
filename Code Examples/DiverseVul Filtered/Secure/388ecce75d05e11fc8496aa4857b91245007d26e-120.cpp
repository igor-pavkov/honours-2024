GF_Err unkn_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 bytesToRead, sub_size, sub_a;
	GF_BitStream *sub_bs;
	GF_UnknownBox *ptr = (GF_UnknownBox *)s;
	if (ptr->size > 0xFFFFFFFF) return GF_ISOM_INVALID_FILE;
	bytesToRead = (u32) (ptr->size);

	if (!bytesToRead) return GF_OK;
	if (bytesToRead>1000000) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Unknown box %s (0x%08X) with payload larger than 1 MBytes, ignoring\n", gf_4cc_to_str(ptr->type), ptr->type ));
		gf_bs_skip_bytes(bs, ptr->dataSize);
		return GF_OK;
	}

	ptr->data = (char*)gf_malloc(bytesToRead);
	if (ptr->data == NULL ) return GF_OUT_OF_MEM;
	ptr->dataSize = bytesToRead;
	gf_bs_read_data(bs, ptr->data, ptr->dataSize);

	//try to parse container boxes, check if next 8 bytes match a subbox
	sub_bs = gf_bs_new(ptr->data, ptr->dataSize, GF_BITSTREAM_READ);
	sub_size = gf_bs_read_u32(sub_bs);
	sub_a = gf_bs_read_u8(sub_bs);
	e = (sub_size && (sub_size <= ptr->dataSize)) ? GF_OK : GF_NOT_SUPPORTED;
	if (! isalnum(sub_a)) e = GF_NOT_SUPPORTED;
	sub_a = gf_bs_read_u8(sub_bs);
	if (! isalnum(sub_a)) e = GF_NOT_SUPPORTED;
	sub_a = gf_bs_read_u8(sub_bs);
	if (! isalnum(sub_a)) e = GF_NOT_SUPPORTED;
	sub_a = gf_bs_read_u8(sub_bs);
	if (! isalnum(sub_a)) e = GF_NOT_SUPPORTED;

	if (e == GF_OK) {
		gf_bs_seek(sub_bs, 0);
		gf_bs_set_cookie(sub_bs, GF_ISOM_BS_COOKIE_NO_LOGS);
		e = gf_isom_box_array_read(s, sub_bs, NULL);
	}
	gf_bs_del(sub_bs);
	if (e==GF_OK) {
		gf_free(ptr->data);
		ptr->data = NULL;
		ptr->dataSize = 0;
	} else if (s->child_boxes) {
		gf_isom_box_array_del(s->child_boxes);
		s->child_boxes=NULL;
	}

	return GF_OK;
}
GF_Err databox_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_DataBox *ptr = (GF_DataBox *)s;

	ptr->reserved = gf_bs_read_int(bs, 32);
	ISOM_DECREASE_SIZE(ptr, 4);

	if (ptr->size) {
		ptr->dataSize = (u32) ptr->size;
		ptr->data = (char*)gf_malloc(ptr->dataSize * sizeof(ptr->data[0]) + 1);
		if (ptr->data == NULL) return GF_OUT_OF_MEM;
		ptr->data[ptr->dataSize] = 0;
		gf_bs_read_data(bs, ptr->data, ptr->dataSize);
	}

	return GF_OK;
}
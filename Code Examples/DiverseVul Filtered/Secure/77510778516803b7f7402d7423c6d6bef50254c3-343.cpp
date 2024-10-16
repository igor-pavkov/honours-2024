GF_Err dfla_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_FLACConfigBox *ptr = (GF_FLACConfigBox *) s;
	ptr->dataSize = (u32) ptr->size;
	ptr->size=0;
	ptr->data = gf_malloc(ptr->dataSize);
	gf_bs_read_data(bs, ptr->data, ptr->dataSize);
	return GF_OK;
}
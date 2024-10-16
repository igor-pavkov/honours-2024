GF_Err name_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 length;
	GF_NameBox *ptr = (GF_NameBox *)s;

	length = (u32) (ptr->size);
	ptr->string = (char*)gf_malloc(sizeof(char) * (length+1));
	if (! ptr->string) return GF_OUT_OF_MEM;

	gf_bs_read_data(bs, ptr->string, length);
	ptr->string[length] = 0;
	return GF_OK;
}
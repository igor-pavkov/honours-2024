GF_Err rely_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_RelyHintBox *ptr = (GF_RelyHintBox *)s;
	ISOM_DECREASE_SIZE(ptr, 1);
	ptr->reserved = gf_bs_read_int(bs, 6);
	ptr->preferred = gf_bs_read_int(bs, 1);
	ptr->required = gf_bs_read_int(bs, 1);
	return GF_OK;
}
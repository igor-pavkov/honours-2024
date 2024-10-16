GF_Err tpay_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TPAYBox *ptr = (GF_TPAYBox *)s;
	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->nbBytes = gf_bs_read_u32(bs);
	return GF_OK;
}
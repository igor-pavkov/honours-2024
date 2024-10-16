GF_Err iKMS_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ISMAKMSBox *ptr = (GF_ISMAKMSBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
    if (ptr->URI)
        gf_bs_write_data(bs, ptr->URI, (u32) strlen(ptr->URI));
    gf_bs_write_u8(bs, 0);
	return GF_OK;
}
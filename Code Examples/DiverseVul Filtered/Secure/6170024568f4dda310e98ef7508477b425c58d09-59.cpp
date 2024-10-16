GF_Err gama_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_GamaInfoBox *ptr = (GF_GamaInfoBox *)s;

	ISOM_DECREASE_SIZE(s, 4);

    ptr->gama = gf_bs_read_u32(bs);
	return GF_OK;
}
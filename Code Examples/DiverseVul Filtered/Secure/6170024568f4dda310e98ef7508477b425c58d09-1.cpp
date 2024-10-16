GF_Err chrm_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ChromaInfoBox *ptr = (GF_ChromaInfoBox *)s;

	ISOM_DECREASE_SIZE(s, 2);

    ptr->chroma = gf_bs_read_u16(bs);
	return GF_OK;
}
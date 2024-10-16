GF_Err ihdr_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_J2KImageHeaderBox *ptr = (GF_J2KImageHeaderBox *) s;

	ISOM_DECREASE_SIZE(s, 14)

	ptr->height = gf_bs_read_u32(bs);
	ptr->width = gf_bs_read_u32(bs);
	ptr->nb_comp = gf_bs_read_u16(bs);
	ptr->bpc = gf_bs_read_u8(bs);
	ptr->Comp = gf_bs_read_u8(bs);
	ptr->UnkC = gf_bs_read_u8(bs);
	ptr->IPR = gf_bs_read_u8(bs);

	return GF_OK;
}
GF_Err fiel_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_FieldInfoBox *ptr = (GF_FieldInfoBox *)s;

	ISOM_DECREASE_SIZE(s, 2);

    ptr->field_count = gf_bs_read_u8(bs);
    ptr->field_order = gf_bs_read_u8(bs);
	return GF_OK;
}
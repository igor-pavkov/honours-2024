GF_Err kind_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_KindBox *ptr = (GF_KindBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
    if (ptr->schemeURI)
        gf_bs_write_data(bs, ptr->schemeURI, (u32) (strlen(ptr->schemeURI) + 1 ));
    else
        gf_bs_write_u8(bs, 0);

    if (ptr->value) {
		gf_bs_write_data(bs, ptr->value, (u32) (strlen(ptr->value) + 1) );
	}
	return GF_OK;
}
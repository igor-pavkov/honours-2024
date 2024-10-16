GF_Err esds_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u8 *enc_desc;
	u32 descSize = 0;
	GF_ESDBox *ptr = (GF_ESDBox *)s;
	//make sure we write with no ESID and no OCRESID
    if (ptr->desc) {
        ptr->desc->ESID = 0;
        ptr->desc->OCRESID = 0;
    }
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	e = gf_odf_desc_write((GF_Descriptor *)ptr->desc, &enc_desc, &descSize);
	if (e) return e;
	gf_bs_write_data(bs, enc_desc, descSize);
	//free our buffer
	gf_free(enc_desc);
	return GF_OK;
}
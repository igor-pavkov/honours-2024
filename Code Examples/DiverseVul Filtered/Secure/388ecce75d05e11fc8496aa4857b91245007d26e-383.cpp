GF_Err cprt_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_CopyrightBox *ptr = (GF_CopyrightBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, 0, 1);
	if (ptr->packedLanguageCode[0]) {
		gf_bs_write_int(bs, ptr->packedLanguageCode[0] - 0x60, 5);
		gf_bs_write_int(bs, ptr->packedLanguageCode[1] - 0x60, 5);
		gf_bs_write_int(bs, ptr->packedLanguageCode[2] - 0x60, 5);
	} else {
		gf_bs_write_int(bs, 0, 15);
	}
	if (ptr->notice) {
		gf_bs_write_data(bs, ptr->notice, (u32) (strlen(ptr->notice) + 1) );
	}
	return GF_OK;
}
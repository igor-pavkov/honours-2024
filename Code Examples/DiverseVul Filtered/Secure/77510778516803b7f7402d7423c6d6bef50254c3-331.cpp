GF_Err tfdt_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TFBaseMediaDecodeTimeBox *ptr = (GF_TFBaseMediaDecodeTimeBox *) s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	if (ptr->version==1) {
		gf_bs_write_u64(bs, ptr->baseMediaDecodeTime);
	} else {
		gf_bs_write_u32(bs, (u32) ptr->baseMediaDecodeTime);
	}
	return GF_OK;
}
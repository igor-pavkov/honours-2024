GF_Err mdri_box_write(GF_Box *s, GF_BitStream *bs)
{
//	GF_OMADRMMutableInformationBox*ptr = (GF_OMADRMMutableInformationBox*)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	return GF_OK;
}
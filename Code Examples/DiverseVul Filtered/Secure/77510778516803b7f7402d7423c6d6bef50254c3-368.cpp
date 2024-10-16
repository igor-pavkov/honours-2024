GF_Err lsrc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_LASERConfigurationBox *ptr = (GF_LASERConfigurationBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, ptr->hdr, ptr->hdr_size);
	return GF_OK;
}
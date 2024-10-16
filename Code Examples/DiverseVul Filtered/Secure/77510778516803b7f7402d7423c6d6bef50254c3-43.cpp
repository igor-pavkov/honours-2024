GF_Err clap_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_CleanApertureBox *ptr = (GF_CleanApertureBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->cleanApertureWidthN);
	gf_bs_write_u32(bs, ptr->cleanApertureWidthD);
	gf_bs_write_u32(bs, ptr->cleanApertureHeightN);
	gf_bs_write_u32(bs, ptr->cleanApertureHeightD);
	gf_bs_write_u32(bs, (u32) ptr->horizOffN);
	gf_bs_write_u32(bs, ptr->horizOffD);
	gf_bs_write_u32(bs, (u32) ptr->vertOffN);
	gf_bs_write_u32(bs, ptr->vertOffD);
	return GF_OK;
}
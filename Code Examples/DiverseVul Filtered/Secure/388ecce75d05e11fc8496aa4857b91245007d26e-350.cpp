GF_Err mdhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_MediaHeaderBox *ptr = (GF_MediaHeaderBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->version == 1) {
		gf_bs_write_u64(bs, ptr->creationTime);
		gf_bs_write_u64(bs, ptr->modificationTime);
		gf_bs_write_u32(bs, ptr->timeScale);
		gf_bs_write_u64(bs, ptr->duration);
	} else {
		gf_bs_write_u32(bs, (u32) ptr->creationTime);
		gf_bs_write_u32(bs, (u32) ptr->modificationTime);
		gf_bs_write_u32(bs, ptr->timeScale);
		gf_bs_write_u32(bs, (u32) ptr->duration);
	}
	//SPECS: BIT(1) of padding
	gf_bs_write_int(bs, 0, 1);
	gf_bs_write_int(bs, ptr->packedLanguage[0] - 0x60, 5);
	gf_bs_write_int(bs, ptr->packedLanguage[1] - 0x60, 5);
	gf_bs_write_int(bs, ptr->packedLanguage[2] - 0x60, 5);
	gf_bs_write_u16(bs, ptr->reserved);
	return GF_OK;
}
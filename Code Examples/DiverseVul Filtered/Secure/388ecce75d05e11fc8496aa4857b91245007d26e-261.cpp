GF_Err ftyp_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_FileTypeBox *ptr = (GF_FileTypeBox *)s;

	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->majorBrand = gf_bs_read_u32(bs);
	ptr->minorVersion = gf_bs_read_u32(bs);

	if (ptr->size % 4) return GF_ISOM_INVALID_FILE;
	ptr->altCount = ( (u32) (ptr->size)) / 4;
	if (!ptr->altCount) return GF_OK;

	ptr->altBrand = (u32*)gf_malloc(sizeof(u32)*ptr->altCount);
	if (!ptr->altBrand) return GF_OUT_OF_MEM;

	for (i = 0; i<ptr->altCount; i++) {
		ptr->altBrand[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;
}
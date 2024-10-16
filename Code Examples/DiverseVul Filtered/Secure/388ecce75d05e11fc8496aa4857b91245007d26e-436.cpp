GF_Err ftyp_box_size(GF_Box *s)
{
	GF_FileTypeBox *ptr = (GF_FileTypeBox *)s;

	ptr->size += 8 + ptr->altCount * 4;
	return GF_OK;
}
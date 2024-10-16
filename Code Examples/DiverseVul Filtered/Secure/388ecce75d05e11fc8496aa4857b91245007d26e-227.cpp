GF_Err mvhd_box_size(GF_Box *s)
{
	GF_MovieHeaderBox *ptr = (GF_MovieHeaderBox *)s;
	if (ptr->duration==(u64) -1) ptr->version = 0;
	else ptr->version = (ptr->duration>0xFFFFFFFF) ? 1 : 0;

	ptr->size += (ptr->version == 1) ? 28 : 16;
	ptr->size += 80;
	return GF_OK;
}
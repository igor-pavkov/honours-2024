GF_Err mdhd_box_size(GF_Box *s)
{
	GF_MediaHeaderBox *ptr = (GF_MediaHeaderBox *)s;
	ptr->version = (ptr->duration>0xFFFFFFFF) ? 1 : 0;

	ptr->size += 4;
	ptr->size += (ptr->version == 1) ? 28 : 16;
	return GF_OK;
}
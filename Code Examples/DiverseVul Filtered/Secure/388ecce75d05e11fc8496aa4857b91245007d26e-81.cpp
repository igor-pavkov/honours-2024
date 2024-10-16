GF_Err tkhd_box_size(GF_Box *s)
{
	GF_TrackHeaderBox *ptr = (GF_TrackHeaderBox *)s;

	if (ptr->duration==(u64) -1) ptr->version = 0;
	else ptr->version = (ptr->duration>0xFFFFFFFF) ? 1 : 0;
	ptr->size += (ptr->version == 1) ? 32 : 20;
	ptr->size += 60;
	return GF_OK;
}
GF_Err gmin_box_size(GF_Box *s)
{
	GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;
	ptr->size += 12;
	return GF_OK;
}
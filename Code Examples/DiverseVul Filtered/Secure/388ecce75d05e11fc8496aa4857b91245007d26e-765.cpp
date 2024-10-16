GF_Err mehd_box_size(GF_Box *s)
{
	GF_MovieExtendsHeaderBox *ptr = (GF_MovieExtendsHeaderBox *)s;
	ptr->version = (ptr->fragment_duration>0xFFFFFFFF) ? 1 : 0;
	s->size += (ptr->version == 1) ? 8 : 4;
	return GF_OK;
}
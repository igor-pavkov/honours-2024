GF_Err svhd_box_size(GF_Box *s)
{
	GF_SphericalVideoInfoBox *ptr = (GF_SphericalVideoInfoBox *)s;
	if (ptr->string)
		s->size += (u32) strlen(ptr->string);
	s->size += 1;
	return GF_OK;
}
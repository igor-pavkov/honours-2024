GF_Err ccst_box_size(GF_Box *s)
{
	GF_CodingConstraintsBox *ptr = (GF_CodingConstraintsBox *)s;
	ptr->size += 4;
	return GF_OK;
}
GF_Err cslg_box_size(GF_Box *s)
{
	GF_CompositionToDecodeBox *ptr = (GF_CompositionToDecodeBox *)s;

	ptr->size += 20;
	return GF_OK;
}
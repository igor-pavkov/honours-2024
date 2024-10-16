GF_Err ctts_box_size(GF_Box *s)
{
	GF_CompositionOffsetBox *ptr = (GF_CompositionOffsetBox *) s;

	ptr->size += 4 + (8 * ptr->nb_entries);
	return GF_OK;
}
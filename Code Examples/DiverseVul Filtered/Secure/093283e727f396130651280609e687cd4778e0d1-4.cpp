GF_Err afra_box_size(GF_Box *s)
{
	GF_AdobeFragRandomAccessBox *ptr = (GF_AdobeFragRandomAccessBox *)s;

	s->size += 9
	           + ptr->entry_count * (ptr->long_offsets ? 16 : 12)
	           + (ptr->global_entries ? 4 + ptr->global_entry_count * (4 + (ptr->long_offsets ? 16 : 8) + (ptr->long_ids ? 8 : 4)) : 0);

	return GF_OK;
}
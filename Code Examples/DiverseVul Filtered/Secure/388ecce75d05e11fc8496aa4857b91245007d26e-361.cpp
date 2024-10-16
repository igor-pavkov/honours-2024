GF_Err stdp_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 entry;
	GF_DegradationPriorityBox *ptr = (GF_DegradationPriorityBox *)s;

	/*out-of-order stdp, assume no padding at the end and take the entire remaining data for entries*/
	if (!ptr->nb_entries) ptr->nb_entries = (u32) ptr->size / 2;
	else if (ptr->nb_entries > ptr->size / 2) return GF_ISOM_INVALID_FILE;

	ptr->priorities = (u16 *) gf_malloc(ptr->nb_entries * sizeof(u16));
	if (ptr->priorities == NULL) return GF_OUT_OF_MEM;
	for (entry = 0; entry < ptr->nb_entries; entry++) {
		ptr->priorities[entry] = gf_bs_read_u16(bs);
	}
	ISOM_DECREASE_SIZE(ptr, (2*ptr->nb_entries) );
	return GF_OK;
}
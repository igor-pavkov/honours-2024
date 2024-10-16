GF_Err elst_box_size(GF_Box *s)
{
	u32 durtimebytes;
	u32 i, nb_entries;
	GF_EditListBox *ptr = (GF_EditListBox *)s;

	//entry count
	ptr->size += 4;
	nb_entries = gf_list_count(ptr->entryList);
	ptr->version = 0;
	for (i=0; i<nb_entries; i++) {
		GF_EdtsEntry *p = (GF_EdtsEntry*)gf_list_get(ptr->entryList, i);
		if ((p->segmentDuration>0xFFFFFFFF) || (p->mediaTime>0xFFFFFFFF)) {
			ptr->version = 1;
			break;
		}
	}
	durtimebytes = (ptr->version == 1 ? 16 : 8) + 4;
	ptr->size += (nb_entries * durtimebytes);
	return GF_OK;
}
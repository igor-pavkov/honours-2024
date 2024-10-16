GF_Err mvcg_box_size(GF_Box *s)
{
	u32 i;
	GF_MultiviewGroupBox *ptr = (GF_MultiviewGroupBox *) s;

	ptr->size += 7;
	for (i=0; i<ptr->num_entries; i++) {
		switch (ptr->entries[i].entry_type) {
		case 0:
			ptr->size += 1 + 4;
			break;
		case 1:
			ptr->size += 1 + 6;
			break;
		case 2:
			ptr->size += 1 + 2;
			break;
		case 3:
			ptr->size += 1 + 4;
			break;
		}
	}
	return GF_OK;
}
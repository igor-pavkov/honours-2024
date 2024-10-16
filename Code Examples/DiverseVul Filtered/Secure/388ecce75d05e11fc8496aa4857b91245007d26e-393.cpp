GF_Err url_box_size(GF_Box *s)
{
	GF_DataEntryURLBox *ptr = (GF_DataEntryURLBox *)s;

	if ( !(ptr->flags & 1)) {
		if (ptr->location) ptr->size += 1 + strlen(ptr->location);
	}
	return GF_OK;
}
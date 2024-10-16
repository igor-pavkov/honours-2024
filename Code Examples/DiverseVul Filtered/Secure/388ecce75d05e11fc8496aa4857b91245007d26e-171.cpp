GF_Err urn_box_size(GF_Box *s)
{
	GF_DataEntryURNBox *ptr = (GF_DataEntryURNBox *)s;

	if ( !(ptr->flags & 1)) {
		if (ptr->nameURN) ptr->size += 1 + strlen(ptr->nameURN);
		if (ptr->location) ptr->size += 1 + strlen(ptr->location);
	}
	return GF_OK;
}
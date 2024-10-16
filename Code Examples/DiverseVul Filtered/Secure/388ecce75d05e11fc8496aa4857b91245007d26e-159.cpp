GF_Err gnrm_box_size(GF_Box *s)
{
	GF_GenericSampleEntryBox *ptr = (GF_GenericSampleEntryBox *)s;
	s->type = GF_ISOM_BOX_TYPE_GNRM;
	ptr->size += 8+ptr->data_size;
	return GF_OK;
}
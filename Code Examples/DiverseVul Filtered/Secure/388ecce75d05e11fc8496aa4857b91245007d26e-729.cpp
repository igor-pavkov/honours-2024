GF_Err gnrv_box_size(GF_Box *s)
{
	GF_GenericVisualSampleEntryBox *ptr = (GF_GenericVisualSampleEntryBox *)s;
	s->type = GF_ISOM_BOX_TYPE_GNRV;
	gf_isom_video_sample_entry_size((GF_VisualSampleEntryBox *)s);
	ptr->size += ptr->data_size;
	return GF_OK;
}
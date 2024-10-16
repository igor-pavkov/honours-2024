GF_Box *gnrv_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_GenericVisualSampleEntryBox, GF_ISOM_BOX_TYPE_GNRV);
	gf_isom_video_sample_entry_init((GF_VisualSampleEntryBox*) tmp);
	return (GF_Box *)tmp;
}
GF_Box *tmcd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TimeCodeSampleEntryBox, GF_QT_BOX_TYPE_TMCD);
	gf_isom_sample_entry_init((GF_SampleEntryBox*)tmp);
	return (GF_Box *)tmp;
}
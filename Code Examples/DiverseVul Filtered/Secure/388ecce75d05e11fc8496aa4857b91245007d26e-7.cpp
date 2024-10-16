GF_Box *gnrm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_GenericSampleEntryBox, GF_ISOM_BOX_TYPE_GNRM);

	gf_isom_sample_entry_init((GF_SampleEntryBox*)tmp);
	return (GF_Box *)tmp;
}
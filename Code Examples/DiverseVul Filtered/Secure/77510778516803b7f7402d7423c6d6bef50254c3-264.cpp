GF_Box *lsr1_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_LASeRSampleEntryBox, GF_ISOM_BOX_TYPE_LSR1);
	gf_isom_sample_entry_init((GF_SampleEntryBox*)tmp);
	return (GF_Box *)tmp;
}
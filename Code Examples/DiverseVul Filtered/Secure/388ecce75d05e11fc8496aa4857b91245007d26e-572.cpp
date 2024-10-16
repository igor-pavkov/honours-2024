GF_Box *gen_sample_entry_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleEntryBox, GF_QT_SUBTYPE_C608);//type will be overriten
	gf_isom_sample_entry_init((GF_SampleEntryBox*)tmp);
	return (GF_Box *)tmp;
}
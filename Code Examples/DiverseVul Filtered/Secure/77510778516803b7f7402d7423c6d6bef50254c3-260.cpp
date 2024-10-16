void metx_box_del(GF_Box *s)
{
	GF_MetaDataSampleEntryBox *ptr = (GF_MetaDataSampleEntryBox*)s;
	if (ptr == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);

	if (ptr->content_encoding) gf_free(ptr->content_encoding);
	if (ptr->xml_namespace) gf_free(ptr->xml_namespace);
	if (ptr->xml_schema_loc) gf_free(ptr->xml_schema_loc);
	if (ptr->mime_type) gf_free(ptr->mime_type);
	gf_free(ptr);
}
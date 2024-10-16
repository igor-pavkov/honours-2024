GF_Err gnrv_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_GenericVisualSampleEntryBox *ptr = (GF_GenericVisualSampleEntryBox *)s;

	//carefull we are not writing the box type but the entry type so switch for write
	ptr->type = ptr->EntryType;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	ptr->type = GF_ISOM_BOX_TYPE_GNRV;

	gf_isom_video_sample_entry_write((GF_VisualSampleEntryBox *)ptr, bs);
	gf_bs_write_data(bs,  ptr->data, ptr->data_size);
	return GF_OK;
}
GF_Err gnrm_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_GenericSampleEntryBox *ptr = (GF_GenericSampleEntryBox *)s;

	//carefull we are not writing the box type but the entry type so switch for write
	ptr->type = ptr->EntryType;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	ptr->type = GF_ISOM_BOX_TYPE_GNRM;
	gf_bs_write_data(bs, ptr->reserved, 6);
	gf_bs_write_u16(bs, ptr->dataReferenceIndex);
	gf_bs_write_data(bs,  ptr->data, ptr->data_size);
	return GF_OK;
}
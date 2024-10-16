GF_Err metx_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_MetaDataSampleEntryBox *ptr = (GF_MetaDataSampleEntryBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_data(bs, ptr->reserved, 6);
	gf_bs_write_u16(bs, ptr->dataReferenceIndex);

	if (ptr->type!=GF_ISOM_BOX_TYPE_STPP) {
		if (ptr->content_encoding)
			gf_bs_write_data(bs, ptr->content_encoding, (u32) strlen(ptr->content_encoding));
		gf_bs_write_u8(bs, 0);
	}

	if ((ptr->type==GF_ISOM_BOX_TYPE_METX) || (ptr->type==GF_ISOM_BOX_TYPE_STPP)) {
		if (ptr->xml_namespace)
			gf_bs_write_data(bs, ptr->xml_namespace, (u32) strlen(ptr->xml_namespace));

		gf_bs_write_u8(bs, 0);

		if (ptr->xml_schema_loc)
			gf_bs_write_data(bs, ptr->xml_schema_loc, (u32) strlen(ptr->xml_schema_loc));
		gf_bs_write_u8(bs, 0);

		if (ptr->type==GF_ISOM_BOX_TYPE_STPP) {
			if (ptr->mime_type)
				gf_bs_write_data(bs, ptr->mime_type, (u32) strlen(ptr->mime_type));

			gf_bs_write_u8(bs, 0);
		}
	}
	//mett, sbtt, stxt
	else {
		if (ptr->mime_type)
			gf_bs_write_data(bs, ptr->mime_type, (u32) strlen(ptr->mime_type));

		gf_bs_write_u8(bs, 0);
	}

	return GF_OK;
}
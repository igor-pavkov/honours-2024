GF_Err minf_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;
	GF_Err e;
	e = gf_isom_box_array_read(s, bs, minf_AddBox);
	if (! ptr->dataInformation) {
		GF_Box *dinf, *dref, *url;
		extern Bool use_dump_mode;
		Bool dump_mode = use_dump_mode;
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing DataInformationBox\n"));
		//commented on purpose, we are still able to handle the file, we only throw an error but keep processing
//		e = GF_ISOM_INVALID_FILE;

		//add a dinf box to avoid any access to a null dinf
		dinf = gf_isom_box_new(GF_ISOM_BOX_TYPE_DINF);
		if (!dinf) return GF_OUT_OF_MEM;
		if (ptr->InfoHeader && gf_list_find(ptr->other_boxes, ptr->InfoHeader)>=0) dump_mode = GF_TRUE;
		if (ptr->sampleTable && gf_list_find(ptr->other_boxes, ptr->sampleTable)>=0) dump_mode = GF_TRUE;

		ptr->dataInformation = (GF_DataInformationBox *)dinf;

		dref = gf_isom_box_new(GF_ISOM_BOX_TYPE_DREF);
		if (!dref) return GF_OUT_OF_MEM;
		e = dinf_AddBox(dinf, dref);

		url = gf_isom_box_new(GF_ISOM_BOX_TYPE_URL);
		if (!url) return GF_OUT_OF_MEM;
		((GF_FullBox*)url)->flags = 1;
		e = gf_isom_box_add_default(dref, url);

		if (dump_mode) {
			gf_isom_box_add_for_dump_mode((GF_Box*)ptr, (GF_Box*)ptr->dataInformation);
			gf_isom_box_add_for_dump_mode((GF_Box*)dinf, (GF_Box*)dref);
		}
	}
	return e;
}
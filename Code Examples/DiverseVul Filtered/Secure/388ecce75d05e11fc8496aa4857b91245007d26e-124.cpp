GF_Err minf_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;
	GF_Err e;

	e = gf_isom_box_array_read(s, bs, minf_on_child_box);

	if (!e && ! ptr->dataInformation) {
		GF_Box *url;
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Missing DataInformationBox\n"));
		//commented on purpose, we are still able to handle the file, we only throw an error but keep processing
//		e = GF_ISOM_INVALID_FILE;

		//add a dinf box to avoid any access to a null dinf
		ptr->dataInformation = (GF_DataInformationBox *) gf_isom_box_new_parent(&ptr->child_boxes, GF_ISOM_BOX_TYPE_DINF);
		if (!ptr->dataInformation) return GF_OUT_OF_MEM;

		ptr->dataInformation->dref = (GF_DataReferenceBox *) gf_isom_box_new_parent(&ptr->dataInformation->child_boxes, GF_ISOM_BOX_TYPE_DREF);
		if (!ptr->dataInformation->dref) return GF_OUT_OF_MEM;

		url = gf_isom_box_new_parent(&ptr->dataInformation->dref->child_boxes, GF_ISOM_BOX_TYPE_URL);
		if (!url) return GF_OUT_OF_MEM;
		((GF_FullBox*)url)->flags = 1;
	}
	return e;
}
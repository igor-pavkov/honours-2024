GF_Err esds_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e=GF_OK;
	u32 descSize;
	GF_ESDBox *ptr = (GF_ESDBox *)s;

	descSize = (u32) (ptr->size);

	if (descSize) {
		char *enc_desc = (char*)gf_malloc(sizeof(char) * descSize);
		if (!enc_desc) return GF_OUT_OF_MEM;
		//get the payload
		gf_bs_read_data(bs, enc_desc, descSize);
		//send it to the OD Codec
		e = gf_odf_desc_read(enc_desc, descSize, (GF_Descriptor **) &ptr->desc);
		//OK, free our desc
		gf_free(enc_desc);

		if (ptr->desc && (ptr->desc->tag!=GF_ODF_ESD_TAG) ) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid descriptor tag 0x%x in esds\n", ptr->desc->tag));
			gf_odf_desc_del((GF_Descriptor*)ptr->desc);
			ptr->desc=NULL;
			return GF_ISOM_INVALID_FILE;
		}

		if (e) {
			ptr->desc = NULL;
		} else {
			/*fix broken files*/
			if (ptr->desc && !ptr->desc->URLString) {
				if (!ptr->desc->slConfig) {
					ptr->desc->slConfig = (GF_SLConfig *) gf_odf_desc_new(GF_ODF_SLC_TAG);
					ptr->desc->slConfig->predefined = SLPredef_MP4;
				} else if (ptr->desc->slConfig->predefined != SLPredef_MP4) {
					ptr->desc->slConfig->predefined = SLPredef_MP4;
					gf_odf_slc_set_pref(ptr->desc->slConfig);
				}
			}
		}
	}
	return e;
}
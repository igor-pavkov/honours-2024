void gf_isom_ipmpx_remove_tool_list(GF_ISOFile *the_file)
{
	/*remove IPMPToolList if any*/
	if (the_file && the_file->moov && the_file->moov->iods && (the_file ->moov->iods->descriptor->tag == GF_ODF_ISOM_IOD_TAG) ) {
		GF_IsomInitialObjectDescriptor *iod = (GF_IsomInitialObjectDescriptor *)the_file ->moov->iods->descriptor;
		if (iod->IPMPToolList) gf_odf_desc_del((GF_Descriptor*) iod->IPMPToolList);
		iod->IPMPToolList = NULL;
	}
}
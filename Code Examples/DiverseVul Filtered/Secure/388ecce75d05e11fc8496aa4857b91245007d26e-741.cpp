GF_Err minf_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_MediaInformationBox *ptr = (GF_MediaInformationBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_NMHD:
	case GF_ISOM_BOX_TYPE_STHD:
	case GF_ISOM_BOX_TYPE_VMHD:
	case GF_ISOM_BOX_TYPE_SMHD:
	case GF_ISOM_BOX_TYPE_HMHD:
	case GF_ISOM_BOX_TYPE_GMHD:
		if (ptr->InfoHeader) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->InfoHeader = a;
		return GF_OK;

	case GF_ISOM_BOX_TYPE_DINF:
		if (ptr->dataInformation) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->dataInformation = (GF_DataInformationBox *)a;
		return GF_OK;

	case GF_ISOM_BOX_TYPE_STBL:
		if (ptr->sampleTable ) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->sampleTable = (GF_SampleTableBox *)a;
		return GF_OK;
	}
	return GF_OK;
}
Bool Media_IsSelfContained(GF_MediaBox *mdia, u32 StreamDescIndex)
{
	u32 drefIndex=0;
	GF_FullBox *a=NULL;
	GF_SampleEntryBox *se = NULL;

	Media_GetSampleDesc(mdia, StreamDescIndex, &se, &drefIndex);
	if (!drefIndex) return 0;
	if (mdia
		&& mdia->information
		&& mdia->information->dataInformation
		&& mdia->information->dataInformation->dref
	) {
		a = (GF_FullBox*)gf_list_get(mdia->information->dataInformation->dref->child_boxes, drefIndex - 1);
	}
	if (!a) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] broken file: Data reference index set to %d but no data reference entry found\n", drefIndex));
		return 1;
	}
	if (a->flags & 1) return 1;
	/*QT specific*/
	if (a->type == GF_QT_BOX_TYPE_ALIS) return 1;
	return 0;
}
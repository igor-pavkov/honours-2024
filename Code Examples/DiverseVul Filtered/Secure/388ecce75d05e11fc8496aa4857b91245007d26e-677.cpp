GF_Err stbl_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_SampleTableBox *ptr = (GF_SampleTableBox *)s;
	if (!a) return GF_OK;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_STTS:
		if (ptr->TimeToSample) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->TimeToSample = (GF_TimeToSampleBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_CTTS:
		if (ptr->CompositionOffset) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->CompositionOffset = (GF_CompositionOffsetBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_CSLG:
		if (ptr->CompositionToDecode) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->CompositionToDecode = (GF_CompositionToDecodeBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_STSS:
		if (ptr->SyncSample) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->SyncSample = (GF_SyncSampleBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_STSD:
		if (ptr->SampleDescription) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->SampleDescription  =(GF_SampleDescriptionBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_STZ2:
	case GF_ISOM_BOX_TYPE_STSZ:
		if (ptr->SampleSize) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->SampleSize = (GF_SampleSizeBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_STSC:
		if (ptr->SampleToChunk) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->SampleToChunk = (GF_SampleToChunkBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_PADB:
		if (ptr->PaddingBits) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->PaddingBits = (GF_PaddingBitsBox *) a;
		break;

	//WARNING: AS THIS MAY CHANGE DYNAMICALLY DURING EDIT,
	case GF_ISOM_BOX_TYPE_CO64:
	case GF_ISOM_BOX_TYPE_STCO:
		if (ptr->ChunkOffset) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->ChunkOffset = a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_STSH:
		if (ptr->ShadowSync) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->ShadowSync = (GF_ShadowSyncBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_STDP:
		if (ptr->DegradationPriority) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->DegradationPriority = (GF_DegradationPriorityBox *)a;
		break;
	case GF_ISOM_BOX_TYPE_SDTP:
		if (ptr->SampleDep) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->SampleDep = (GF_SampleDependencyTypeBox *)a;
		break;

	case GF_ISOM_BOX_TYPE_SUBS:
		if (!ptr->sub_samples) ptr->sub_samples = gf_list_new();
		gf_list_add(ptr->sub_samples, a);
		//check subsample box
		{
			GF_SubSampleInformationBox *subs = (GF_SubSampleInformationBox *)a;
			GF_SubSampleInfoEntry *ent = gf_list_get(subs->Samples, 0);
			if (!ent) {
				gf_list_rem(subs->Samples, 0);
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] first entry in SubSample in track SampleTable is invalid\n"));
			}
			else if (ent->sample_delta==0) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] first entry in SubSample in track SampleTable has sample_delta of 0, should be one. Fixing\n"));
				ent->sample_delta = 1;
			}
		}
		break;

	case GF_ISOM_BOX_TYPE_SBGP:
		if (!ptr->sampleGroups) ptr->sampleGroups = gf_list_new();
		gf_list_add(ptr->sampleGroups, a);
		break;
	case GF_ISOM_BOX_TYPE_SGPD:
		if (!ptr->sampleGroupsDescription) ptr->sampleGroupsDescription = gf_list_new();
		gf_list_add(ptr->sampleGroupsDescription, a);
		break;

	case GF_ISOM_BOX_TYPE_SAIZ:
		if (!ptr->sai_sizes) ptr->sai_sizes = gf_list_new();
		gf_list_add(ptr->sai_sizes, a);
		break;
	case GF_ISOM_BOX_TYPE_SAIO:
		if (!ptr->sai_offsets) ptr->sai_offsets = gf_list_new();
		gf_list_add(ptr->sai_offsets, a);
		break;
	}
	return GF_OK;
}
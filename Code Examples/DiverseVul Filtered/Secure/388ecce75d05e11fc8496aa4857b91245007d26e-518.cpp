GF_Err traf_on_child_box(GF_Box *s, GF_Box *a)
{
	GF_TrackFragmentBox *ptr = (GF_TrackFragmentBox *)s;

	switch (a->type) {
	case GF_ISOM_BOX_TYPE_TFHD:
		if (ptr->tfhd) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->tfhd = (GF_TrackFragmentHeaderBox *) a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TRUN:
		return gf_list_add(ptr->TrackRuns, a);
	case GF_ISOM_BOX_TYPE_SDTP:
		if (ptr->sdtp) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->sdtp = (GF_SampleDependencyTypeBox *)a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_TFDT:
		if (ptr->tfdt) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->tfdt = (GF_TFBaseMediaDecodeTimeBox*) a;
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SUBS:
		if (!ptr->sub_samples) ptr->sub_samples = gf_list_new();
		return gf_list_add(ptr->sub_samples, a);
	case GF_ISOM_BOX_TYPE_SBGP:
		if (!ptr->sampleGroups) ptr->sampleGroups = gf_list_new();
		gf_list_add(ptr->sampleGroups, a);
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SGPD:
		if (!ptr->sampleGroupsDescription) ptr->sampleGroupsDescription = gf_list_new();
		gf_list_add(ptr->sampleGroupsDescription, a);
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SAIZ:
		if (!ptr->sai_sizes) ptr->sai_sizes = gf_list_new();
		gf_list_add(ptr->sai_sizes, a);
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SAIO:
		if (!ptr->sai_offsets) ptr->sai_offsets = gf_list_new();
		gf_list_add(ptr->sai_offsets, a);
		return GF_OK;
	//we will throw an error if both PIFF_PSEC and SENC are found. Not such files seen yet
	case GF_ISOM_BOX_TYPE_UUID:
		if ( ((GF_UUIDBox *)a)->internal_4cc==GF_ISOM_BOX_UUID_PSEC) {
			if (ptr->sample_encryption) ERROR_ON_DUPLICATED_BOX(a, ptr)
			ptr->sample_encryption = (GF_SampleEncryptionBox *)a;
			ptr->sample_encryption->traf = ptr;
			return GF_OK;
		} else {
			return GF_OK;
		}
	case GF_ISOM_BOX_TYPE_SENC:
		if (ptr->sample_encryption) ERROR_ON_DUPLICATED_BOX(a, ptr)
		ptr->sample_encryption = (GF_SampleEncryptionBox *)a;
		ptr->sample_encryption->traf = ptr;
		return GF_OK;
	}
	return GF_OK;
}
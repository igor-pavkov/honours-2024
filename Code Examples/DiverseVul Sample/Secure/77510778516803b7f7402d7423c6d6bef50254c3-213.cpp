GF_Err fiin_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	FDItemInformationBox *ptr = (FDItemInformationBox *)s;
	switch(a->type) {
	case GF_ISOM_BOX_TYPE_PAEN:
		BOX_FIELD_LIST_ASSIGN(partition_entries)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_SEGR:
		BOX_FIELD_ASSIGN(session_info, FDSessionGroupBox)
		return GF_OK;
	case GF_ISOM_BOX_TYPE_GITN:
		BOX_FIELD_ASSIGN(group_id_to_name, GroupIdToNameBox)
		return GF_OK;
	}
	return GF_OK;
}
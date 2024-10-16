GF_Box *udta_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_UserDataBox, GF_ISOM_BOX_TYPE_UDTA);
	tmp->recordList = gf_list_new();
	if (!tmp->recordList) {
		gf_free(tmp);
		return NULL;
	}
	return (GF_Box *)tmp;
}
GF_Box *fdsa_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_HintSample, GF_ISOM_BOX_TYPE_FDSA);
	if (!tmp) return NULL;
	tmp->packetTable = gf_list_new();
	tmp->hint_subtype = GF_ISOM_BOX_TYPE_FDP_STSD;
	return (GF_Box*)tmp;
}
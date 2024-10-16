GF_Err gf_isom_linf_read_entry(void *entry, GF_BitStream *bs)
{
	GF_LHVCLayerInformation* ptr = (GF_LHVCLayerInformation *)entry;
	u32 i, count;

	if (!ptr) return GF_BAD_PARAM;
	gf_bs_read_int(bs, 2);
	count = gf_bs_read_int(bs, 6);
	for (i = 0; i < count; i++) {
		LHVCLayerInfoItem *li;
		GF_SAFEALLOC(li, LHVCLayerInfoItem);
		if (!li) return GF_OUT_OF_MEM;
		gf_bs_read_int(bs, 4);
		li->layer_id = gf_bs_read_int(bs, 6);
		li->min_TemporalId = gf_bs_read_int(bs, 3);
		li->max_TemporalId = gf_bs_read_int(bs, 3);
		gf_bs_read_int(bs, 1);
		li->sub_layer_presence_flags = gf_bs_read_int(bs, 7);
		gf_list_add(ptr->num_layers_in_track, li);
	}
	return GF_OK;
}
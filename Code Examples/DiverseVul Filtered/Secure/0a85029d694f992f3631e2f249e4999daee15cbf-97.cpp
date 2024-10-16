GF_Err gf_isom_linf_write_entry(void *entry, GF_BitStream *bs)
{
	GF_LHVCLayerInformation* ptr = (GF_LHVCLayerInformation *)entry;
	u32 i, count;
	if (!ptr) return GF_OK;

	gf_bs_write_int(bs, 0, 2);
	count=gf_list_count(ptr->num_layers_in_track);
	gf_bs_write_int(bs, count, 6);
	for (i = 0; i < count; i++) {
		LHVCLayerInfoItem *li = (LHVCLayerInfoItem *)gf_list_get(ptr->num_layers_in_track, i);
		gf_bs_write_int(bs, 0, 4);
		gf_bs_write_int(bs, li->layer_id, 6);
		gf_bs_write_int(bs, li->min_TemporalId, 3);
		gf_bs_write_int(bs, li->max_TemporalId, 3);
		gf_bs_write_int(bs, 0, 1);
		gf_bs_write_int(bs, li->sub_layer_presence_flags, 7);
	}
	return GF_OK;
}
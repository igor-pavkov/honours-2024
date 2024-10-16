void gf_isom_linf_del_entry(void *entry)
{
	GF_LHVCLayerInformation* ptr = (GF_LHVCLayerInformation *)entry;
	if (!ptr) return;
	while (gf_list_count(ptr->num_layers_in_track)) {
		LHVCLayerInfoItem *li = (LHVCLayerInfoItem *)gf_list_get(ptr->num_layers_in_track, 0);
		gf_free(li);
		gf_list_rem(ptr->num_layers_in_track, 0);
	}
	gf_list_del(ptr->num_layers_in_track);
	gf_free(ptr);
	return;
}
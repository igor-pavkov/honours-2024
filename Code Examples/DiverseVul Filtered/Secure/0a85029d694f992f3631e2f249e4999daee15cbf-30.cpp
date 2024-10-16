u32 gf_isom_linf_size_entry(void *entry)
{
	GF_LHVCLayerInformation* ptr = (GF_LHVCLayerInformation *)entry;
	u32 size = 0, count;
	if (!ptr) return 0;

	size += 1;
	count=gf_list_count(ptr->num_layers_in_track);
	size += count * 3;
	return size;
}
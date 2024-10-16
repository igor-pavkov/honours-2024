GF_LHVCLayerInformation *gf_isom_linf_new_entry()
{
	GF_LHVCLayerInformation* ptr;
	GF_SAFEALLOC(ptr, GF_LHVCLayerInformation);
	if (ptr) ptr->num_layers_in_track = gf_list_new();

	return ptr;

}
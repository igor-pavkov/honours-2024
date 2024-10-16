static GF_Err gf_isom_check_mvc(GF_ISOFile *the_file, GF_TrackBox *trak, GF_MPEGVisualSampleEntryBox *entry)
{
	u32 i;
	GF_Box *mvci;
	GF_MultiviewGroupBox *mvcg;
	GF_ViewIdentifierBox *vwid;

	if (entry->mvc_config) {}
	else if (entry->avc_config && entry->avc_config->config && entry->avc_config->config->sequenceParameterSetExtensions) {}
	else
		return GF_OK;

	mvci = gf_isom_box_find_child(trak->Media->information->child_boxes, GF_ISOM_BOX_TYPE_MVCI);
	if (!mvci) {
		mvci = gf_isom_box_new_parent(&trak->Media->information->child_boxes, GF_ISOM_BOX_TYPE_MVCI);
		if (!mvci) return GF_OUT_OF_MEM;
	}
	mvcg = (GF_MultiviewGroupBox *) gf_isom_box_find_child(mvci->child_boxes, GF_ISOM_BOX_TYPE_MVCG);
	if (!mvcg) {
		mvcg = (GF_MultiviewGroupBox *)gf_isom_box_new_parent(&mvci->child_boxes, GF_ISOM_BOX_TYPE_MVCG);
		if (!mvcg) return GF_OUT_OF_MEM;
	}
	//this is very crude, we should try to parse the bitstream to fill these
	mvcg->num_entries = 0;
	if (mvcg->entries) {
		gf_free(mvcg->entries);
		mvcg->entries = NULL;
	}
	if (entry->avc_config) {
		if (gf_list_count(entry->avc_config->config->sequenceParameterSets))
			mvcg->num_entries += 1;
		mvcg->num_entries += gf_list_count(entry->avc_config->config->sequenceParameterSetExtensions);
	}
	if (entry->mvc_config && entry->mvc_config->config) {
		mvcg->num_entries += gf_list_count(entry->mvc_config->config->sequenceParameterSets);
	}
	mvcg->entries = gf_malloc(sizeof(MVCIEntry)*mvcg->num_entries);
	if (!mvcg->entries) return GF_OUT_OF_MEM;
	memset(mvcg->entries, 0, sizeof(MVCIEntry)*mvcg->num_entries);
	for (i=0; i<mvcg->num_entries; i++) {
		mvcg->entries[i].entry_type = 2;
		mvcg->entries[i].output_view_id = i;
	}
	vwid = (GF_ViewIdentifierBox *) gf_isom_box_find_child(entry->child_boxes, GF_ISOM_BOX_TYPE_VWID);
	if (!vwid) {
		vwid = (GF_ViewIdentifierBox *)gf_isom_box_new_parent(&entry->child_boxes, GF_ISOM_BOX_TYPE_VWID);
		if (!mvcg) return GF_OUT_OF_MEM;
	}
	if (vwid->views) gf_free(vwid->views);
	vwid->num_views = mvcg->num_entries;
	vwid->views = gf_malloc(sizeof(ViewIDEntry)*vwid->num_views);
	if (!vwid->views) return GF_OUT_OF_MEM;
	memset(vwid->views, 0, sizeof(ViewIDEntry)*vwid->num_views);

	for (i=0; i<vwid->num_views; i++) {
		vwid->views[i].base_view_type = i ? 0 : 1;
		vwid->views[i].view_id = i;
		vwid->views[i].view_order_index = i;
	}

	return GF_OK;
}
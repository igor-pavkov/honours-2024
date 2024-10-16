GF_Err gf_isom_remove_samp_enc_box(GF_ISOFile *the_file, u32 trackNumber)
{
	u32 i;
	GF_SampleTableBox *stbl;
	GF_TrackBox *trak = gf_isom_get_track_from_file(the_file, trackNumber);
	if (!trak) return GF_BAD_PARAM;
	stbl = trak->Media->information->sampleTable;
	if (!stbl)
		return GF_BAD_PARAM;

	for (i = 0; i < gf_list_count(stbl->child_boxes); i++) {
		GF_Box *a = (GF_Box *)gf_list_get(stbl->child_boxes, i);
		if ((a->type ==GF_ISOM_BOX_TYPE_UUID) && (((GF_UUIDBox *)a)->internal_4cc == GF_ISOM_BOX_UUID_PSEC)) {
			gf_isom_box_del_parent(&stbl->child_boxes, a);
			i--;
		}
		else if (a->type == GF_ISOM_BOX_TYPE_SENC) {
			gf_isom_box_del_parent(&stbl->child_boxes, a);
			i--;
		}
	}

	if (!gf_list_count(stbl->child_boxes)) {
		gf_list_del(stbl->child_boxes);
		stbl->child_boxes = NULL;
	}
	for (i = 0; i < gf_list_count(trak->child_boxes); i++) {
		GF_Box *a = (GF_Box *)gf_list_get(trak->child_boxes, i);
		if ((a->type ==GF_ISOM_BOX_TYPE_UUID) && (((GF_UUIDBox *)a)->internal_4cc == GF_ISOM_BOX_UUID_PSEC)) {
			gf_isom_box_del_parent(&trak->child_boxes, a);
			i--;
		}
		else if (a->type == GF_ISOM_BOX_TYPE_SENC) {
			gf_isom_box_del_parent(&trak->child_boxes, a);
			i--;
		}
	}
	return GF_OK;
}
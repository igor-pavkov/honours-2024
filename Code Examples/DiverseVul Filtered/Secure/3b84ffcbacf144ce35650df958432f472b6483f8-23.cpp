GF_Err gf_isom_remove_pssh_box(GF_ISOFile *the_file)
{
	u32 i;
	for (i = 0; i < gf_list_count(the_file->moov->child_boxes); i++) {
		GF_Box *a = (GF_Box *)gf_list_get(the_file->moov->child_boxes, i);
		GF_UUIDBox *uuid = (GF_UUIDBox *)a;
		if ((a->type == GF_ISOM_BOX_TYPE_PSSH)
			|| ((a->type == GF_ISOM_BOX_TYPE_UUID) && (uuid->internal_4cc == GF_ISOM_BOX_UUID_PSSH))
		) {
			gf_isom_box_del_parent(&the_file->moov->child_boxes, a);
			i--;
		}
	}

	if (!gf_list_count(the_file->moov->child_boxes)) {
		gf_list_del(the_file->moov->child_boxes);
		the_file->moov->child_boxes = NULL;
	}

	return GF_OK;
}
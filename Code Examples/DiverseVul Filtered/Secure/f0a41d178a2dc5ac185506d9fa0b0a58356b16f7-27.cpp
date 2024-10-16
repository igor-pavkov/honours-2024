u32 gf_isom_get_pssh_count(GF_ISOFile *file)
{
	u32 count=0;
	u32 i=0;
	GF_Box *a_box;
	if (file->moov) {
		while ((a_box = (GF_Box*)gf_list_enum(file->moov->child_boxes, &i))) {
			if (a_box->type != GF_ISOM_BOX_TYPE_PSSH) continue;
			count++;
		}
	}
	if (file->meta) {
		while ((a_box = (GF_Box*)gf_list_enum(file->meta->child_boxes, &i))) {
			if (a_box->type != GF_ISOM_BOX_TYPE_PSSH) continue;
			count++;
		}
	}
	return count;
}
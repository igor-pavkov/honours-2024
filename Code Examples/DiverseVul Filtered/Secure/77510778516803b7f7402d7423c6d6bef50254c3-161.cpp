GF_Err sidx_box_size(GF_Box *s)
{
	GF_SegmentIndexBox *ptr = (GF_SegmentIndexBox*) s;

	ptr->size += 12;
	if (ptr->version==0) {
		ptr->size += 8;
	} else {
		ptr->size += 16;
	}
	ptr->size += ptr->nb_refs * 12;
	return GF_OK;
}
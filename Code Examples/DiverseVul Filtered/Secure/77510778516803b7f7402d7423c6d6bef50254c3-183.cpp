GF_Err vwid_box_size(GF_Box *s)
{
	u32 i;
	GF_ViewIdentifierBox *ptr = (GF_ViewIdentifierBox *) s;
	ptr->size += 3;
	for (i=0; i<ptr->num_views; i++) {
		ptr->size += 6 + 2 * ptr->views[i].num_ref_views;
	}
	return GF_OK;
}
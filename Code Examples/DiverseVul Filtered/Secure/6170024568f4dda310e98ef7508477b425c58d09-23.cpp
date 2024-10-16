GF_Err ilst_item_box_size(GF_Box *s)
{
	GF_ListItemBox *ptr = (GF_ListItemBox *)s;

	/*generic box list*/
	if (ptr->child_boxes && !ptr->data) {
	}
	/*iTune way: data-box-encapsulated box list*/
	else if (ptr->data && !ptr->data->qt_style) {
		u32 pos=0;
		gf_isom_check_position(s, (GF_Box* ) ptr->data, &pos);
	}
	/*QT way: raw data*/
	else if (ptr->data) {
		ptr->size += ptr->data->dataSize + 4;
	}
	return GF_OK;
}
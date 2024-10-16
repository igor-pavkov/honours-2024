GF_Err ilst_item_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ListItemBox *ptr = (GF_ListItemBox *) s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	/*generic box list*/
	if (ptr->child_boxes && !ptr->data) {
	}
	/*iTune way: data-box-encapsulated box list*/
	else if (!ptr->data->qt_style) {
	}
	/*QT way: raw data*/
	else {
		gf_bs_write_u16(bs, ptr->data->dataSize);
		gf_bs_write_u16(bs, 0);
		gf_bs_write_data(bs, ptr->data->data, ptr->data->dataSize);
		ptr->size = 0; //abort
	}
	return GF_OK;
}
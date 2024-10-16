static GF_Node *lsr_read_cursorManager(GF_LASeRCodec *lsr)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_LSR_cursorManager);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt,TAG_SVG_ATT_x, 1, "x");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y, 1, "y");
	lsr_read_href(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
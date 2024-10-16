static GF_Node *lsr_read_linearGradient(GF_LASeRCodec *lsr)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_linearGradient);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);
	lsr_read_gradient_units(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x1, 1, "x1");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x2, 1, "x2");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y1, 1, "y1");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y2, 1, "y2");
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
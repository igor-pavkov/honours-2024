static GF_Node *lsr_read_radialGradient(GF_LASeRCodec *lsr)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_radialGradient);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_cx, 1, "cx");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_cy, 1, "cy");
	lsr_read_gradient_units(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_r, 1, "r");
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
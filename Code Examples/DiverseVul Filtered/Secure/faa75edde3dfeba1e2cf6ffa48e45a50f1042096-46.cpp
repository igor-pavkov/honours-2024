static GF_Node *lsr_read_image(GF_LASeRCodec *lsr)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_image);
	lsr_read_id(lsr, elt);
	lsr_read_rare_full(lsr, elt);
	lsr_read_eRR(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_height, 1, "height");
	lsr_read_opacity(lsr, elt);

	lsr_read_preserve_aspect_ratio(lsr, elt);
	lsr_read_content_type(lsr, elt);
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_width, 1, "width");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x, 1, "x");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y, 1, "y");
	lsr_read_href(lsr, elt);
	lsr_read_transform_behavior(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
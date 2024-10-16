static GF_Node *lsr_read_rect(GF_LASeRCodec *lsr, u32 same_type)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_rect);

	if (same_type) {
		if (lsr->prev_rect) {
			lsr_restore_base(lsr, (SVG_Element*)elt, (SVG_Element *)lsr->prev_rect, (same_type==2) ? 1 : 0, 0);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] samerect coded in bitstream but no rect defined !\n"));
		}
		lsr_read_id(lsr, elt);
		if (same_type==2) lsr_read_fill(lsr, elt);
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_height, 0, "height");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_width, 0, "width");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x, 1, "x");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y, 1, "y");
	} else {
		lsr_read_id(lsr, elt);
		lsr_read_rare_full(lsr, elt);
		lsr_read_fill(lsr, elt);
		lsr_read_stroke(lsr, elt);
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_height, 0, "height");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_rx, 1, "rx");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_ry, 1, "ry");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_width, 0, "width");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x, 1, "x");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y, 1, "y");
		lsr_read_any_attribute(lsr, elt, 1);
		lsr->prev_rect = (SVG_Element*)elt;
	}
	lsr_read_group_content(lsr, elt, same_type);
	return elt;
}
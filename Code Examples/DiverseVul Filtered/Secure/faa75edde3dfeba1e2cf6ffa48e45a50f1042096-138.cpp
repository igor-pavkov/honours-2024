static GF_Node *lsr_read_line(GF_LASeRCodec *lsr, Bool is_same)
{
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_line);

	if (is_same) {
		if (lsr->prev_line) {
			lsr_restore_base(lsr, (SVG_Element*) elt, (SVG_Element *)lsr->prev_line, 0, 0);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] sameline coded in bitstream but no line defined !\n"));
		}
		lsr_read_id(lsr, elt);
	} else {
		lsr_read_id(lsr, elt);
		lsr_read_rare_full(lsr, elt);
		lsr_read_fill(lsr, elt);
		lsr_read_stroke(lsr, elt);
	}
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x1, 1, "x1");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x2, 0, "x2");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y1, 1, "y1");
	lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y2, 0, "y2");
	if (!is_same) {
		lsr_read_any_attribute(lsr, elt, 1);
		lsr->prev_line = (SVG_Element*)elt;
	}
	lsr_read_group_content(lsr, elt, is_same);
	return elt;
}
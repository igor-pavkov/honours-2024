static GF_Node *lsr_read_text(GF_LASeRCodec *lsr, u32 same_type)
{
	u32 flag;
	GF_FieldInfo info;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_text);
	if (same_type) {
		if (lsr->prev_text) {
			lsr_restore_base(lsr, (SVG_Element *)elt, (SVG_Element *)lsr->prev_text, (same_type==2) ? 1 : 0, 0);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] sametext coded in bitstream but no text defined !\n"));
		}
		lsr_read_id(lsr, elt);
		if (same_type==2) lsr_read_fill(lsr, elt);
		lsr_read_coord_list(lsr, elt, TAG_SVG_ATT_text_x, "x");
		lsr_read_coord_list(lsr, elt, TAG_SVG_ATT_text_y, "y");
	} else {
		lsr_read_id(lsr, elt);
		lsr_read_rare_full(lsr, elt);
		lsr_read_fill(lsr, elt);
		lsr_read_stroke(lsr, elt);

		GF_LSR_READ_INT(lsr, flag, 1, "editable");
		if (flag) {
			lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_editable, 1, 0, &info);
			*(SVG_Boolean*)info.far_ptr = flag;
		}
		lsr_read_float_list(lsr, elt, TAG_SVG_ATT_text_rotate, NULL, "rotate");
		lsr_read_coord_list(lsr, elt, TAG_SVG_ATT_text_x, "x");
		lsr_read_coord_list(lsr, elt, TAG_SVG_ATT_text_y, "y");
		lsr_read_any_attribute(lsr, elt, 1);
		lsr->prev_text = (SVG_Element*)elt;
	}
	lsr_read_group_content(lsr, elt, same_type);
	return elt;
}
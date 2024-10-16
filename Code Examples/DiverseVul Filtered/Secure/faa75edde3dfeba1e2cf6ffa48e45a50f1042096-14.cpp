static GF_Node *lsr_read_use(GF_LASeRCodec *lsr, Bool is_same)
{
	GF_FieldInfo info;
	u32 flag;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_use);
	if (is_same) {
		if (lsr->prev_use) {
			lsr_restore_base(lsr, (SVG_Element *)elt, lsr->prev_use, 0, 0);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] sameuse coded in bitstream but no use defined !\n"));
		}
		lsr_read_id(lsr, elt);
		lsr_read_href(lsr, elt);
	} else {
		lsr_read_id(lsr, elt);
		lsr_read_rare_full(lsr, elt);
		lsr_read_fill(lsr, elt);
		lsr_read_stroke(lsr, elt);
		lsr_read_eRR(lsr, elt);

		GF_LSR_READ_INT(lsr, flag, 1, "hasOverflow");
		if (flag) {
			lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_overflow, 1, 0, &info);
			GF_LSR_READ_INT(lsr, *(SVG_Overflow*)info.far_ptr, 2, "overflow");
		}
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_x, 1, "x");
		lsr_read_coordinate_ptr(lsr, elt, TAG_SVG_ATT_y, 1, "y");
		lsr_read_href(lsr, elt);
		lsr_read_any_attribute(lsr, elt, 1);
		lsr->prev_use = (SVG_Element*)elt;
	}
	lsr_read_group_content(lsr, elt, is_same);
	return elt;
}
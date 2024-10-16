static GF_Node *lsr_read_path(GF_LASeRCodec *lsr, u32 same_type)
{
	u32 flag;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_path);

	if (same_type) {
		if (lsr->prev_path) {
			lsr_restore_base(lsr, (SVG_Element*)elt, (SVG_Element *)lsr->prev_path, (same_type==2) ? 1 : 0, 0);
		} else {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CODING, ("[LASeR] samepath coded in bitstream but no path defined !\n"));
		}
		lsr_read_id(lsr, elt);
		if (same_type==2) lsr_read_fill(lsr, elt);
		lsr_read_path_type(lsr, elt, TAG_SVG_ATT_d, NULL, "d");
	} else {
		lsr_read_id(lsr, elt);
		lsr_read_rare_full(lsr, elt);
		lsr_read_fill(lsr, elt);
		lsr_read_stroke(lsr, elt);
		lsr_read_path_type(lsr, elt, TAG_SVG_ATT_d, NULL, "d");
		GF_LSR_READ_INT(lsr, flag, 1, "hasPathLength");
		if (flag) {
			GF_FieldInfo info;
			lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_pathLength, 1, 0, &info);
			((SVG_Number*)info.far_ptr)->value = lsr_read_fixed_16_8(lsr, "pathLength");
		}
		lsr_read_any_attribute(lsr, elt, 1);
		lsr->prev_path = (SVG_Element*)elt;
	}
	lsr_read_group_content(lsr, elt, same_type);
	return elt;
}
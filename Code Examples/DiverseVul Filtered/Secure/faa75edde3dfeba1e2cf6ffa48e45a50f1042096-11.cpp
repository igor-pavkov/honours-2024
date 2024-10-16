static GF_Node *lsr_read_a(GF_LASeRCodec *lsr)
{
	Bool flag;
	GF_Node *elt = (GF_Node*) gf_node_new(lsr->sg, TAG_SVG_a);
	lsr_read_id(lsr, elt);
	lsr_read_rare_full(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);
	lsr_read_eRR(lsr, elt);
	GF_LSR_READ_INT(lsr, flag, 1, "hasTarget");
	if (flag) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_target, 1, 0, &info);
		lsr_read_byte_align_string(lsr, info.far_ptr, "target");
	}
	lsr_read_href(lsr, elt);
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
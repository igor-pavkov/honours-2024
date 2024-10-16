static GF_Node *lsr_read_stop(GF_LASeRCodec *lsr)
{
	GF_FieldInfo info;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_SVG_stop);
	lsr_read_id(lsr, elt);
	lsr_read_rare(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);

	lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_offset, 1, 0, &info);
	((SVG_Number*)info.far_ptr)->value = lsr_read_fixed_16_8(lsr, "offset");
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
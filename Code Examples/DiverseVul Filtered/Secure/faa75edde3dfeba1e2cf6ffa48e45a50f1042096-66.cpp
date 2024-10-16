static GF_Node *lsr_read_rectClip(GF_LASeRCodec *lsr)
{
	u32 flag;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_LSR_rectClip);
	lsr_read_id(lsr, elt);
	lsr_read_rare_full(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);
	lsr_read_eRR(lsr, elt);
	GF_LSR_READ_INT(lsr, flag, 1, "has_size");
	if (flag) {
		SVG_Number num;
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_size, 1, 0, &info);
		lsr_read_coordinate(lsr, & num, 0, "width");
		((LASeR_Size*)info.far_ptr)->width = num.value;
		lsr_read_coordinate(lsr, & num, 0, "height");
		((LASeR_Size*)info.far_ptr)->height = num.value;
	}
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
static GF_Node *lsr_read_selector(GF_LASeRCodec *lsr)
{
	u32 flag;
	GF_Node *elt = gf_node_new(lsr->sg, TAG_LSR_selector);
	lsr_read_id(lsr, elt);
	lsr_read_rare_full(lsr, elt);
	lsr_read_fill(lsr, elt);
	lsr_read_stroke(lsr, elt);
	lsr_read_eRR(lsr, elt);
	GF_LSR_READ_INT(lsr, flag, 1, "hasChoice");
	if (flag) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_choice, 1, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "choice");
		if (flag) {
			GF_LSR_READ_INT(lsr, ((LASeR_Choice*)info.far_ptr)->type, 1, "type");
		} else {
			GF_LSR_READ_INT(lsr, ((LASeR_Choice*)info.far_ptr)->choice_index, 8, "value");
			((LASeR_Choice*)info.far_ptr)->type = LASeR_CHOICE_N;
		}
	}
	lsr_read_any_attribute(lsr, elt, 1);
	lsr_read_group_content(lsr, elt, 0);
	return elt;
}
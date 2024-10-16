static void lsr_read_anim_values_ex(GF_LASeRCodec *lsr, GF_Node *n, u32 *tr_type)
{
	u32 flag, i, count = 0;
	u32 coded_type;
	GF_FieldInfo info;
	SMIL_AnimateValues *values;

	GF_LSR_READ_INT(lsr, flag, 1, "values");
	if (!flag) return;

	lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_values, GF_TRUE, 0, &info);
	values = (SMIL_AnimateValues *)info.far_ptr;

	GF_LSR_READ_INT(lsr, coded_type, 4, "type");
	values->type = coded_type;
	values->laser_strings = 0;

	count = lsr_read_vluimsbf5(lsr, "count");
	for (i=0; i<count; i++) {
		void *att = lsr_read_an_anim_value(lsr, coded_type, "a_value");
		if (att) gf_list_add(values->values, att);
		if (lsr->last_error) return;
	}
	if (tr_type) {
		lsr_translate_anim_trans_values(lsr, info.far_ptr, *tr_type);
	}
}
static void lsr_read_anim_value_ex(GF_LASeRCodec *lsr, GF_Node *n, u32 tag, const char *name, u32 *tr_type)
{
	u32 val, coded_type;
	GF_LSR_READ_INT(lsr, val, 1, name);
	if (val) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, tag, GF_TRUE, 0, &info);

		GF_LSR_READ_INT(lsr, coded_type, 4, "type");
		((SMIL_AnimateValue*)info.far_ptr)->value = lsr_read_an_anim_value(lsr, coded_type, name);
		((SMIL_AnimateValue*)info.far_ptr)->type = coded_type;

		if (tr_type) {
			lsr_translate_anim_trans_value(lsr, info.far_ptr, *tr_type);
		}
	}
}
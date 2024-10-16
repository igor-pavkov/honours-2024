static void lsr_read_attribute_name_ex(GF_LASeRCodec *lsr, GF_Node *n, Bool skippable)
{
	u32 val = 1;
	if (skippable) {
		GF_LSR_READ_INT(lsr, val, 1, "hasAttributeName");
		if (!val) return;
	}

	GF_LSR_READ_INT(lsr, val, 1, "choice");
	if (val) {
		lsr_read_vluimsbf5(lsr, "item[i]");
		lsr_read_vluimsbf5(lsr, "item[i]");
		return;
	} else {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_attributeName, GF_TRUE, GF_FALSE, &info);
		GF_LSR_READ_INT(lsr, val, 8, "attributeType");

		/*translate type to attribute tag*/
		((SMIL_AttributeName*)info.far_ptr)->type = gf_lsr_anim_type_to_attribute(val);
	}
}
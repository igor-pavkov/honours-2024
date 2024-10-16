static void lsr_read_attribute_type(GF_LASeRCodec *lsr, GF_Node *elt)
{
	GF_FieldInfo info;
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "hasAttributeType");
	if (!flag) return;
	lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_attributeType, 1, 0, &info);
	GF_LSR_READ_INT(lsr, *(SMIL_AttributeType*)info.far_ptr, 2, "attributeType");
}
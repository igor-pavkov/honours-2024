static void lsr_read_gradient_units(GF_LASeRCodec *lsr, GF_Node *elt)
{
	u32 flag;
	GF_FieldInfo info;
	GF_LSR_READ_INT(lsr, flag, 1, "hasGradientUnits");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_gradientUnits, 1, 0, &info);
		GF_LSR_READ_INT(lsr, *(SVG_GradientUnit*)info.far_ptr, 1, "gradientUnits");
	}
}
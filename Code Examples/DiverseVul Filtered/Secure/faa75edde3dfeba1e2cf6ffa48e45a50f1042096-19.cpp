static void lsr_read_rotate_type(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "rotate");
	if (flag) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_rotate, GF_TRUE, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "choice");

		if (flag) {
			GF_LSR_READ_INT(lsr, flag, 1, "rotate");
			((SVG_Number *)info.far_ptr)->type = flag ? SVG_NUMBER_AUTO_REVERSE : SVG_NUMBER_AUTO;
		} else {
			((SVG_Number *)info.far_ptr)->value = lsr_read_fixed_16_8(lsr, "rotate");
			((SVG_Number *)info.far_ptr)->type = SVG_NUMBER_VALUE;
		}
	}
}
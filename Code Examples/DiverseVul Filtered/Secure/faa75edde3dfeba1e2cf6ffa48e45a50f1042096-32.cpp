static void lsr_read_calc_mode(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 v;
	/*SMIL_CALCMODE_LINEAR is default and 0 in our code*/
	GF_LSR_READ_INT(lsr, v, 1, "has_calcMode");
	if (v) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_calcMode, GF_TRUE, GF_FALSE, &info);
		GF_LSR_READ_INT(lsr, *(SMIL_CalcMode*)info.far_ptr, 2, "calcMode");
	}
}
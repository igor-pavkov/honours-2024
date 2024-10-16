static void lsr_read_anim_restart(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 1, "has_restart");
	if (val) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_restart, GF_TRUE, 0, &info);
		/*enumeration always{0} never{1} whenNotActive{2}*/
		GF_LSR_READ_INT(lsr, *(SMIL_Restart*)info.far_ptr, 2, "restart");
	}
}
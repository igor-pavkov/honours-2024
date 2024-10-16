static void lsr_read_script_type(GF_LASeRCodec *lsr, GF_Node *n)
{
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "hasType");
	if (flag) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_XLINK_ATT_type, GF_TRUE, 0, &info);
		GF_LSR_READ_INT(lsr, flag, 1, "choice");
		if (flag) {
			GF_LSR_READ_INT(lsr, flag, 1, "script");
			switch (flag) {
			case 0:
				*(SVG_String*)info.far_ptr = gf_strdup("application/ecmascript");
				break;
			case 1:
				*(SVG_String*)info.far_ptr = gf_strdup("application/jar-archive");
				break;
			default:
				break;
			}
		} else {
			lsr_read_byte_align_string(lsr, info.far_ptr, "type");
		}
	}
}
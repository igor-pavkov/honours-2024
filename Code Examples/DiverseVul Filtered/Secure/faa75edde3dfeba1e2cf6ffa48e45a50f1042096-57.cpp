static void lsr_read_transform_behavior(GF_LASeRCodec *lsr, GF_Node *n)
{
	GF_FieldInfo info;
	u32 flag;
	GF_LSR_READ_INT(lsr, flag, 1, "hasTransformBehavior");
	if (flag) {
		lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_transformBehavior, GF_TRUE, 0, &info);
		GF_LSR_READ_INT(lsr, *(SVG_TransformBehavior*)info.far_ptr, 4, "transformBehavior");
	}
}
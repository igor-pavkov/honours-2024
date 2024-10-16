static void lsr_read_fraction_12(GF_LASeRCodec *lsr, GF_Node *elt, u32 tag, const char *name)
{
	GF_FieldInfo info;
	u32 i, count;
	GF_LSR_READ_INT(lsr, count, 1, name);
	if (!count) return;

	lsr->last_error = gf_node_get_attribute_by_tag(elt, tag, GF_TRUE, 0, &info);

	count = lsr_read_vluimsbf5(lsr, "name");
	for (i=0; i<count; i++) {
		Fixed *f = lsr_read_fraction_12_item(lsr);
		gf_list_add( *((SMIL_KeyTimes*)info.far_ptr), f);
		if (lsr->last_error) return;
	}
}
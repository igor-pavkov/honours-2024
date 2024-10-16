static void lsr_read_eRR(GF_LASeRCodec *lsr, GF_Node *elt)
{
	u32 err;
	GF_LSR_READ_INT(lsr, err, 1, "externalResourcesRequired");
	if (err) {
		GF_FieldInfo info;
		lsr->last_error = gf_node_get_attribute_by_tag(elt, TAG_SVG_ATT_externalResourcesRequired, 1, 0, &info);
		*(SVG_Boolean*)info.far_ptr = 1;
	}
}
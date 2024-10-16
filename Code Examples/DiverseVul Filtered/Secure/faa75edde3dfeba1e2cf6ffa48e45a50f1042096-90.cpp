static void lsr_read_object_content(GF_LASeRCodec *lsr, SVG_Element *elt)
{
	u32 val;
	GF_LSR_READ_INT(lsr, val, 1, "has_private_attr");
	if (val) lsr_read_private_attribute_container(lsr);
}
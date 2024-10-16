static u32 lsr_get_attribute_name(GF_LASeRCodec *lsr)
{
	u32 val = 1;
	GF_LSR_READ_INT(lsr, val, 1, "has_attributeName");
	if (!val) return -1;

	GF_LSR_READ_INT(lsr, val, 1, "choice");
	if (val) {
		lsr_read_vluimsbf5(lsr, "item[i]");
		lsr_read_vluimsbf5(lsr, "item[i]");
		return -1;
	} else {
		GF_LSR_READ_INT(lsr, val, 8, "attributeName");
		return val;
	}
}
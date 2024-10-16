static void lsr_read_any_attribute(GF_LASeRCodec *lsr, GF_Node *node, Bool skippable)
{
	u32 val = 1;
	if (skippable) GF_LSR_READ_INT(lsr, val, 1, "has_attrs");
	if (val) {
		do {
			GF_LSR_READ_INT(lsr, val, lsr->info->cfg.extensionIDBits, "reserved");
			val = lsr_read_vluimsbf5(lsr, "len");//len in BITS
			GF_LSR_READ_INT(lsr, val, val, "reserved_val");
			GF_LSR_READ_INT(lsr, val, 1, "hasNextExtension");
			if (lsr->last_error) return;
		} while (val);
	}
}
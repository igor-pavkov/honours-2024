static int do_handle_item(deark *c, lctx *d, i64 opcode_pos, i64 opcode,
						   i64 data_pos, i64 *data_bytes_used)
{
	const char *opcode_name;
	const struct opcode_info *opi;
	i64 n;
	struct pict_rect tmprect;
	int ret = 0;

	*data_bytes_used = 0;

	opi = find_opcode_info(opcode);
	if(opi && opi->name) opcode_name = opi->name;
	else opcode_name = "?";

	if(d->version==2)
		de_dbg(c, "opcode 0x%04x (%s) at %d", (unsigned int)opcode, opcode_name, (int)opcode_pos);
	else
		de_dbg(c, "opcode 0x%02x (%s) at %d", (unsigned int)opcode, opcode_name, (int)opcode_pos);

	if(opi && opi->fn) {
		de_dbg_indent(c, 1);
		*data_bytes_used = opi->size; // Default to the size in the table.
		ret = opi->fn(c, d, opcode, data_pos, data_bytes_used);
		de_dbg_indent(c, -1);
	}
	else if(opi && opi->size_code==SZCODE_EXACT) {
		*data_bytes_used = opi->size;
		ret = 1;
	}
	else if(opi && opi->size_code==SZCODE_REGION) {
		n = de_getu16be(data_pos);
		de_dbg_indent(c, 1);
		de_dbg(c, "region size: %d", (int)n);
		if(n>=10) {
			pict_read_rect(c->infile, data_pos+2, &tmprect, "rect");
		}
		de_dbg_indent(c, -1);
		*data_bytes_used = n;
		ret = 1;
	}
	else if(opi && opi->size_code==SZCODE_POLYGON) {
		n = de_getu16be(data_pos);
		de_dbg_indent(c, 1);
		de_dbg(c, "polygon size: %d", (int)n);
		de_dbg_indent(c, -1);
		*data_bytes_used = n;
		ret = 1;
	}
	else if(opcode>=0x2c && opcode<=0x2f) {
		// Starts with 2-byte size, size does not include the "size" field.
		n = de_getu16be(data_pos);
		*data_bytes_used = 2+n;
		ret = 1;
	}
	else if(opcode>=0x8100 && opcode<=0xffff) {
		// Starts with 4-byte size, size does not include the "size" field.
		n = de_getu32be(data_pos);
		*data_bytes_used = 4+n;
		ret = 1;
	}
	else {
		de_err(c, "Unsupported opcode: 0x%04x", (unsigned int)opcode);
	}

	return ret;
}
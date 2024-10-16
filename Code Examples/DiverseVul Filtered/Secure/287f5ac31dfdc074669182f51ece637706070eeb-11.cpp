static int handler_a1(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 kind;
	i64 len;

	kind = de_getu16be(data_pos);
	len = de_getu16be(data_pos+2);
	de_dbg(c, "comment kind: %d, size: %d", (int)kind, (int)len);
	*bytes_used = 4+len;

	if(kind==100 && len>=4) {
		struct de_fourcc sig4cc;

		dbuf_read_fourcc(c->infile, data_pos+4, &sig4cc, 4, 0x0);
		de_dbg(c, "application comment, signature=0x%08x '%s'",
			(unsigned int)sig4cc.id, sig4cc.id_dbgstr);
		de_dbg_hexdump(c, c->infile, data_pos+8, len-4, 256, NULL, 0x1);
	}
	else if(kind==224) {
		do_iccprofile_item(c, d, data_pos+4, len);
	}
	else {
		de_dbg_hexdump(c, c->infile, data_pos+4, len, 256, NULL, 0x1);
	}

	return 1;
}
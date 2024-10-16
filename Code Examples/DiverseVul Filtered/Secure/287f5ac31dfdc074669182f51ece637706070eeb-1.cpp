static int handler_a0(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 kind;
	kind = de_getu16be(data_pos);
	de_dbg(c, "comment kind: %d", (int)kind);
	return 1;
}
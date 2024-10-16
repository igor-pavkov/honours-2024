static int handler_Rectangle(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	struct pict_rect rect;

	pict_read_rect(c->infile, data_pos, &rect, "rect");
	return 1;
}
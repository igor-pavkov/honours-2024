static void pict_read_point(dbuf *f, i64 pos,
	struct pict_point *point, const char *dbgname)
{
	point->y = dbuf_geti16be(f, pos);
	point->x = dbuf_geti16be(f, pos+2);

	if(dbgname) {
		de_dbg(f->c, "%s: (%d,%d)", dbgname, (int)point->x, (int)point->y);
	}
}
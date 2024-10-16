R_API void r_egg_finalize(REgg *egg) {
	struct egg_patch_t *ep;
	RListIter *iter;
	if (!egg->bin) {
		r_buf_free (egg->bin);
		egg->bin = r_buf_new ();
	}
	r_list_foreach (egg->patches, iter, ep) {
		if (ep->off < 0) {
			ut64 sz;
			const ut8 *buf = r_buf_data (ep->b, &sz);
			r_egg_append_bytes (egg, buf, sz);
		} else if (ep->off < r_buf_size (egg->bin)) {
			ut64 sz;
			const ut8 *buf = r_buf_data (ep->b, &sz);
			int r = r_buf_write_at (egg->bin, ep->off, buf, sz);
			if (r < sz) {
				eprintf ("Error during patch\n");
				return;
			}
		} else {
			eprintf ("Cannot patch outside\n");
			return;
		}
	}
}
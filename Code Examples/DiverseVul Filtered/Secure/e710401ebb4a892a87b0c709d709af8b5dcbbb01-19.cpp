R_API int r_egg_patch(REgg *egg, int off, const ut8 *buf, int len) {
	struct egg_patch_t *ep = R_NEW (struct egg_patch_t);
	if (!ep) {
		return false;
	}
	ep->b = r_buf_new_with_bytes (buf, len);
	if (!ep->b) {
		egg_patch_free (ep);
		return false;
	}
	ep->off = off;
	r_list_append (egg->patches, ep);
	return true;
}
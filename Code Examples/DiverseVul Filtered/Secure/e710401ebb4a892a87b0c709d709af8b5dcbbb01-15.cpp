void egg_patch_free (void *p) {
	struct egg_patch_t *ep = (struct egg_patch_t *)p;
	r_buf_free (ep->b);
	free (ep);
}
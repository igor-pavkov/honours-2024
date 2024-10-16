static void inactivity_cb(uev_t *w, void *arg, int events)
{
	uev_ctx_t *ctx = (uev_ctx_t *)arg;

	INFO("Inactivity timer, exiting ...");
	uev_exit(ctx);
}
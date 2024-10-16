static void pptp_serv_close(struct triton_context_t *ctx)
{
	struct pptp_serv_t *s=container_of(ctx,typeof(*s),ctx);
	triton_md_unregister_handler(&s->hnd, 1);
	triton_context_unregister(ctx);
}
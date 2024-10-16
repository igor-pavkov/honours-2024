static void pptp_ctx_switch(struct triton_context_t *ctx, void *arg)
{
	if (arg) {
		struct ap_session *s = arg;
		net = s->net;
	} else
		net = def_net;
	log_switch(ctx, arg);
}
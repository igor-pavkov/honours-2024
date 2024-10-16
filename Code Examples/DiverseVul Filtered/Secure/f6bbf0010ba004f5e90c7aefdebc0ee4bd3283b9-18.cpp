static irqreturn_t vhost_vdpa_config_cb(void *private)
{
	struct vhost_vdpa *v = private;
	struct eventfd_ctx *config_ctx = v->config_ctx;

	if (config_ctx)
		eventfd_signal(config_ctx, 1);

	return IRQ_HANDLED;
}
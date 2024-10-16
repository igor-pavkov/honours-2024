static int irda_open_lsap(struct irda_sock *self, int pid)
{
	notify_t notify;

	if (self->lsap) {
		IRDA_WARNING("%s(), busy!\n", __func__);
		return -EBUSY;
	}

	/* Initialize callbacks to be used by the IrDA stack */
	irda_notify_init(&notify);
	notify.udata_indication	= irda_data_indication;
	notify.instance = self;
	strncpy(notify.name, "Ultra", NOTIFY_MAX_NAME);

	self->lsap = irlmp_open_lsap(LSAP_CONNLESS, &notify, pid);
	if (self->lsap == NULL) {
		IRDA_DEBUG( 0, "%s(), Unable to allocate LSAP!\n", __func__);
		return -ENOMEM;
	}

	return 0;
}
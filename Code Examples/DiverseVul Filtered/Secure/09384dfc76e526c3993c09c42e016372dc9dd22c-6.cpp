static void irda_destroy_socket(struct irda_sock *self)
{
	IRDA_DEBUG(2, "%s(%p)\n", __func__, self);

	/* Unregister with IrLMP */
	irlmp_unregister_client(self->ckey);
	irlmp_unregister_service(self->skey);

	/* Unregister with LM-IAS */
	if (self->ias_obj) {
		irias_delete_object(self->ias_obj);
		self->ias_obj = NULL;
	}

	if (self->iriap) {
		iriap_close(self->iriap);
		self->iriap = NULL;
	}

	if (self->tsap) {
		irttp_disconnect_request(self->tsap, NULL, P_NORMAL);
		irttp_close_tsap(self->tsap);
		self->tsap = NULL;
	}
#ifdef CONFIG_IRDA_ULTRA
	if (self->lsap) {
		irlmp_close_lsap(self->lsap);
		self->lsap = NULL;
	}
#endif /* CONFIG_IRDA_ULTRA */
}
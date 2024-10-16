static int eaptls_detach(void *arg)
{
	EAP_TLS_CONF	 *conf;
	eap_tls_t 	 *inst;

	inst = (eap_tls_t *) arg;
	conf = inst->conf;

	if (conf) {
		memset(conf, 0, sizeof(*conf));
		free(inst->conf);
		inst->conf = NULL;
	}

	if (inst->ctx) SSL_CTX_free(inst->ctx);
	inst->ctx = NULL;

#ifdef HAVE_OPENSSL_OCSP_H
	if (inst->store) X509_STORE_free(inst->store);
	inst->store = NULL;
#endif

	free(inst);

	return 0;
}
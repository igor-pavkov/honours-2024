static int eaptls_attach(CONF_SECTION *cs, void **instance)
{
	EAP_TLS_CONF	 *conf;
	eap_tls_t 	 *inst;

	/* Store all these values in the data structure for later references */
	inst = (eap_tls_t *)malloc(sizeof(*inst));
	if (!inst) {
		radlog(L_ERR, "rlm_eap_tls: out of memory");
		return -1;
	}
	memset(inst, 0, sizeof(*inst));

	/*
	 *	Parse the config file & get all the configured values
	 */
	conf = (EAP_TLS_CONF *)malloc(sizeof(*conf));
	if (conf == NULL) {
		free(inst);
		radlog(L_ERR, "rlm_eap_tls: out of memory");
		return -1;
	}
	memset(conf, 0, sizeof(*conf));

	inst->conf = conf;
	if (cf_section_parse(cs, conf, module_config) < 0) {
		eaptls_detach(inst);
		return -1;
	}

	/*
	 *	The EAP RFC's say 1020, but we're less picky.
	 */
	if (conf->fragment_size < 100) {
		radlog(L_ERR, "rlm_eap_tls: Fragment size is too small.");
		eaptls_detach(inst);
		return -1;
	}

	/*
	 *	The maximum size for a RADIUS packet is 4096,
	 *	minus the header (20), Message-Authenticator (18),
	 *	and State (18), etc. results in about 4000 bytes of data
	 *	that can be devoted *solely* to EAP.
	 */
	if (conf->fragment_size > 4000) {
		radlog(L_ERR, "rlm_eap_tls: Fragment size is too large.");
		eaptls_detach(inst);
		return -1;
	}

	/*
	 *	Account for the EAP header (4), and the EAP-TLS header
	 *	(6), as per Section 4.2 of RFC 2716.  What's left is
	 *	the maximum amount of data we read from a TLS buffer.
	 */
	conf->fragment_size -= 10;

	/*
	 *	This magic makes the administrators life HUGELY easier
	 *	on initial deployments.
	 *
	 *	If the server starts up in debugging mode, AND the
	 *	bootstrap command is configured, AND it exists, AND
	 *	there is no server certificate
	 */
	if (conf->make_cert_command && (debug_flag >= 2)) {
		struct stat buf;

		if ((stat(conf->make_cert_command, &buf) == 0) &&
		    (stat(conf->certificate_file, &buf) < 0) &&
		    (errno == ENOENT) &&
		    (radius_exec_program(conf->make_cert_command, NULL, 1,
					 NULL, 0, NULL, NULL, 0) != 0)) {
			eaptls_detach(inst);
			return -1;
		}
	}


	/*
	 *	Initialize TLS
	 */
	inst->ctx = init_tls_ctx(conf);
	if (inst->ctx == NULL) {
		eaptls_detach(inst);
		return -1;
	}

#ifdef HAVE_OPENSSL_OCSP_H
	/*
	 * 	Initialize OCSP Revocation Store
	 */
	if (conf->ocsp_enable) {
		inst->store = init_revocation_store(conf);
		if (inst->store == NULL) {
			eaptls_detach(inst);
		  return -1;
		}
	}
#endif /*HAVE_OPENSSL_OCSP_H*/

	if (load_dh_params(inst->ctx, conf->dh_file) < 0) {
		eaptls_detach(inst);
		return -1;
	}

        if (generate_eph_rsa_key(inst->ctx) < 0) {
		eaptls_detach(inst);
                return -1;
        }

	if (conf->verify_tmp_dir) {
		if (chmod(conf->verify_tmp_dir, S_IRWXU) < 0) {
			radlog(L_ERR, "rlm_eap_tls: Failed changing permissions on %s: %s", conf->verify_tmp_dir, strerror(errno));
			eaptls_detach(inst);
			return -1;
		}
	}

	if (conf->verify_client_cert_cmd && !conf->verify_tmp_dir) {
		radlog(L_ERR, "rlm_eap_tls: You MUST set the verify directory in order to use verify_client_cmd");
		eaptls_detach(inst);
		return -1;
	}

	*instance = inst;

	return 0;
}
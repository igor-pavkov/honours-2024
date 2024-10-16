static int ocsp_check(X509_STORE *store, X509 *issuer_cert, X509 *client_cert,
		      EAP_TLS_CONF *conf)
{
	OCSP_CERTID *certid;
	OCSP_REQUEST *req;
	OCSP_RESPONSE *resp = NULL;
	OCSP_BASICRESP *bresp = NULL;
	char *host = NULL;
	char *port = NULL;
	char *path = NULL;
	int use_ssl = -1;
	long nsec = MAX_VALIDITY_PERIOD, maxage = -1;
	BIO *cbio, *bio_out;
	int ocsp_ok = 0;
	int status ;
	ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
	int reason;
#if OPENSSL_VERSION_NUMBER >= 0x1000003f
	OCSP_REQ_CTX *ctx;
	int rc;
	struct timeval now;
	struct timeval when;
#endif

	/* 
	 * Create OCSP Request 
	 */
	certid = OCSP_cert_to_id(NULL, client_cert, issuer_cert);
	req = OCSP_REQUEST_new();
	OCSP_request_add0_id(req, certid);
	if(conf->ocsp_use_nonce){
		OCSP_request_add1_nonce(req, NULL, 8);
	}
	
	/* 
	 * Send OCSP Request and get OCSP Response
	 */

	/* Get OCSP responder URL */ 
	if(conf->ocsp_override_url) {
		OCSP_parse_url(conf->ocsp_url, &host, &port, &path, &use_ssl);
	}
	else {
		ocsp_parse_cert_url(client_cert, &host, &port, &path, &use_ssl);
	}
	
	DEBUG2("[ocsp] --> Responder URL = http://%s:%s%s", host, port, path);

	/* Setup BIO socket to OCSP responder */
	cbio = BIO_new_connect(host);

	bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);

	BIO_set_conn_port(cbio, port);
#if OPENSSL_VERSION_NUMBER < 0x1000003f
	BIO_do_connect(cbio);
 
	/* Send OCSP request and wait for response */
	resp = OCSP_sendreq_bio(cbio, path, req);
	if (!resp) {
		radlog(L_ERR, "Error: Couldn't get OCSP response");
		ocsp_ok = 2;
		goto ocsp_end;
	}
#else
	if (conf->ocsp_timeout)
		BIO_set_nbio(cbio, 1);

	rc = BIO_do_connect(cbio);
	if ((rc <= 0) && ((!conf->ocsp_timeout) || !BIO_should_retry(cbio))) {
		radlog(L_ERR, "Error: Couldn't connect to OCSP responder");
		ocsp_ok = 2;
		goto ocsp_end;
	}

	ctx = OCSP_sendreq_new(cbio, path, req, -1);
	if (!ctx) {
		radlog(L_ERR, "Error: Couldn't send OCSP request");
		ocsp_ok = 2;
		goto ocsp_end;
	}

	gettimeofday(&when, NULL);
	when.tv_sec += conf->ocsp_timeout;

	do {
		rc = OCSP_sendreq_nbio(&resp, ctx);
		if (conf->ocsp_timeout) {
			gettimeofday(&now, NULL);
			if (!timercmp(&now, &when, <))
				break;
		}
	} while ((rc == -1) && BIO_should_retry(cbio));

	if (conf->ocsp_timeout && (rc == -1) && BIO_should_retry(cbio)) {
		radlog(L_ERR, "Error: OCSP response timed out");
		ocsp_ok = 2;
		goto ocsp_end;
	}

	OCSP_REQ_CTX_free(ctx);

	if (rc == 0) {
		radlog(L_ERR, "Error: Couldn't get OCSP response");
		ocsp_ok = 2;
		goto ocsp_end;
	}
#endif

	/* Verify OCSP response status */
	status = OCSP_response_status(resp);
	DEBUG2("[ocsp] --> Response status: %s",OCSP_response_status_str(status));
	if(status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
		radlog(L_ERR, "Error: OCSP response status: %s", OCSP_response_status_str(status));
		goto ocsp_end;
	}
	bresp = OCSP_response_get1_basic(resp);
	if(conf->ocsp_use_nonce && OCSP_check_nonce(req, bresp)!=1) {
		radlog(L_ERR, "Error: OCSP response has wrong nonce value");
		goto ocsp_end;
	}
	if(OCSP_basic_verify(bresp, NULL, store, 0)!=1){
		radlog(L_ERR, "Error: Couldn't verify OCSP basic response");
		goto ocsp_end;
	}
	/*	Verify OCSP cert status */
	if(!OCSP_resp_find_status(bresp, certid, &status, &reason,
				                      &rev, &thisupd, &nextupd)) {
		radlog(L_ERR, "ERROR: No Status found.\n");
		goto ocsp_end;
	}

	if (!OCSP_check_validity(thisupd, nextupd, nsec, maxage)) {
		BIO_puts(bio_out, "WARNING: Status times invalid.\n");
		ERR_print_errors(bio_out);
		goto ocsp_end;
	}
	BIO_puts(bio_out, "\tThis Update: ");
        ASN1_GENERALIZEDTIME_print(bio_out, thisupd);
        BIO_puts(bio_out, "\n");
	if (nextupd) {
		BIO_puts(bio_out, "\tNext Update: ");
		ASN1_GENERALIZEDTIME_print(bio_out, nextupd);
		BIO_puts(bio_out, "\n");
	}

	switch (status) {
	case V_OCSP_CERTSTATUS_GOOD:
		DEBUG2("[oscp] --> Cert status: good");
		ocsp_ok = 1; 
		break;

	default:
		/* REVOKED / UNKNOWN */
		DEBUG2("[ocsp] --> Cert status: %s",OCSP_cert_status_str(status));
                if (reason != -1)
			DEBUG2("[ocsp] --> Reason: %s", OCSP_crl_reason_str(reason));
                BIO_puts(bio_out, "\tRevocation Time: ");
                ASN1_GENERALIZEDTIME_print(bio_out, rev);
                BIO_puts(bio_out, "\n"); 
		break;
	}

ocsp_end:
	/* Free OCSP Stuff */
	OCSP_REQUEST_free(req);
	OCSP_RESPONSE_free(resp);
	free(host);
	free(port);
	free(path);
	BIO_free_all(cbio);
	OCSP_BASICRESP_free(bresp);

	switch (ocsp_ok) {
	case 1:
		DEBUG2("[ocsp] --> Certificate is valid!");
		break;
	case 2:
		if (conf->ocsp_softfail) {
			DEBUG2("[ocsp] --> Unable to check certificate; assuming valid.");
			DEBUG2("[ocsp] --> Warning! This may be insecure.");
			ocsp_ok = 1;
		} else {
			DEBUG2("[ocsp] --> Unable to check certificate; failing!");
			ocsp_ok = 0;
		}
		break;
	default:
		DEBUG2("[ocsp] --> Certificate has been expired/revoked!");
		break;
	}

	return ocsp_ok;
}
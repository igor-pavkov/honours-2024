static kdc_code kdc_process(struct kdc_server *kdc,
			    TALLOC_CTX *mem_ctx,
			    DATA_BLOB *input,
			    DATA_BLOB *reply,
			    struct tsocket_address *peer_addr,
			    struct tsocket_address *my_addr,
			    int datagram_reply)
{
	int ret;
	char *pa;
	struct sockaddr_storage ss;
	krb5_data k5_reply;
	krb5_kdc_configuration *kdc_config =
		(krb5_kdc_configuration *)kdc->private_data;

	krb5_data_zero(&k5_reply);

	krb5_kdc_update_time(NULL);

	ret = tsocket_address_bsd_sockaddr(peer_addr, (struct sockaddr *) &ss,
				sizeof(struct sockaddr_storage));
	if (ret < 0) {
		return KDC_ERROR;
	}
	pa = tsocket_address_string(peer_addr, mem_ctx);
	if (pa == NULL) {
		return KDC_ERROR;
	}

	DBG_DEBUG("Received KDC packet of length %lu from %s\n",
		  (long)input->length - 4, pa);

	ret = krb5_kdc_process_krb5_request(kdc->smb_krb5_context->krb5_context,
					    kdc_config,
					    input->data, input->length,
					    &k5_reply,
					    pa,
					    (struct sockaddr *) &ss,
					    datagram_reply);
	if (ret == -1) {
		*reply = data_blob(NULL, 0);
		return KDC_ERROR;
	}

	if (ret == HDB_ERR_NOT_FOUND_HERE) {
		*reply = data_blob(NULL, 0);
		return KDC_PROXY_REQUEST;
	}

	if (k5_reply.length) {
		*reply = data_blob_talloc(mem_ctx, k5_reply.data, k5_reply.length);
		krb5_data_free(&k5_reply);
	} else {
		*reply = data_blob(NULL, 0);
	}
	return KDC_OK;
}
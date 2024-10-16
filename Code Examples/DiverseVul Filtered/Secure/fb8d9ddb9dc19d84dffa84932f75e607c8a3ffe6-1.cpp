int tls1_get_server_supplemental_data(SSL *s)
	{
	int al = 0;
	int ok;
	long n;
	const unsigned char *p, *d;
	unsigned short supp_data_entry_type = 0;
	unsigned short supp_data_entry_len = 0;
	unsigned long supp_data_len = 0;
	size_t i;
	int cb_retval = 0;

	n=s->method->ssl_get_message(s,
				     SSL3_ST_CR_SUPPLEMENTAL_DATA_A,
				     SSL3_ST_CR_SUPPLEMENTAL_DATA_B,
				     SSL3_MT_SUPPLEMENTAL_DATA,
				     /* use default limit */
				     TLSEXT_MAXLEN_supplemental_data,
				     &ok);

	if (!ok) return((int)n);

	p = (unsigned char *)s->init_msg;
	d = p;
	/* The message cannot be empty */
	if (n < 3)
		{
		al = SSL_AD_DECODE_ERROR;
		SSLerr(SSL_F_TLS1_GET_SERVER_SUPPLEMENTAL_DATA,SSL_R_LENGTH_MISMATCH);
		goto f_err;
		}
	n2l3(p, supp_data_len);
	while (p < d+supp_data_len)
		{
		n2s(p, supp_data_entry_type);
		n2s(p, supp_data_entry_len);
		/* if there is a callback for this supp data type, send it */
		for (i=0; i < s->ctx->cli_supp_data_records_count; i++)
			{
			if (s->ctx->cli_supp_data_records[i].supp_data_type == supp_data_entry_type &&
			    s->ctx->cli_supp_data_records[i].fn1)
				{
				cb_retval = s->ctx->cli_supp_data_records[i].fn1(s, supp_data_entry_type, p,
										 supp_data_entry_len, &al,
										 s->ctx->cli_supp_data_records[i].arg);
				if (cb_retval == 0)
					{
					SSLerr(SSL_F_TLS1_GET_SERVER_SUPPLEMENTAL_DATA, ERR_R_SSL_LIB);
					goto f_err;
					}
				}
			}
		p += supp_data_entry_len;
		}
	return 1;
f_err:
	ssl3_send_alert(s,SSL3_AL_FATAL,al);
	return -1;
	}
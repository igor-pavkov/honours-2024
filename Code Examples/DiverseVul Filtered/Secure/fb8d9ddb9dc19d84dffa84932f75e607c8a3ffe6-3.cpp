int tls1_send_client_supplemental_data(SSL *s, int *skip)
	{
	int al = 0;
	if (s->ctx->cli_supp_data_records_count)
		{
		unsigned char *p = NULL;
		unsigned char *size_loc = NULL;
		cli_supp_data_record *record = NULL;
		size_t length = 0;
		size_t i = 0;

		for (i = 0; i < s->ctx->cli_supp_data_records_count; i++)
			{
			const unsigned char *out = NULL;
			unsigned short outlen = 0;
			int cb_retval = 0;
			record = &s->ctx->cli_supp_data_records[i];

			/* NULL callback or -1 omits supp data entry*/
			if (!record->fn2)
				continue;
			cb_retval = record->fn2(s, record->supp_data_type,
						&out, &outlen, &al,
						record->arg);
			if (cb_retval == -1)
				continue; /* skip this supp data entry */
			if (cb_retval == 0)
				{
				SSLerr(SSL_F_TLS1_SEND_CLIENT_SUPPLEMENTAL_DATA,ERR_R_BUF_LIB);
				goto f_err;
				}
			if (outlen == 0 || TLSEXT_MAXLEN_supplemental_data < outlen + 4 + length)
				{
				SSLerr(SSL_F_TLS1_SEND_CLIENT_SUPPLEMENTAL_DATA,ERR_R_BUF_LIB);
				return 0;
				}
			/* if first entry, write handshake message type */
			if (length == 0)
				{
				if (!BUF_MEM_grow_clean(s->init_buf, 4))
					{
					SSLerr(SSL_F_TLS1_SEND_CLIENT_SUPPLEMENTAL_DATA,ERR_R_BUF_LIB);
					return 0;
					}
				p = (unsigned char *)s->init_buf->data;
				*(p++) = SSL3_MT_SUPPLEMENTAL_DATA;
				/* update message length when all
				 * callbacks complete */
				size_loc = p;
				/* skip over handshake length field (3
				 * bytes) and supp_data length field
				 * (3 bytes) */
				p += 3 + 3;
				length += 1 +3 +3;
				}
			if (!BUF_MEM_grow(s->init_buf, outlen + 4))
				{
				SSLerr(SSL_F_TLS1_SEND_CLIENT_SUPPLEMENTAL_DATA,ERR_R_BUF_LIB);
				return 0;
				}
			s2n(record->supp_data_type, p);
			s2n(outlen, p);
			memcpy(p, out, outlen);
			length += (outlen + 4);
			p += outlen;
			}
		if (length > 0)
			{
			/* write handshake length */
			l2n3(length - 4, size_loc);
			/* supp_data length */
			l2n3(length - 7, size_loc);
			s->state = SSL3_ST_CW_SUPPLEMENTAL_DATA_B;
			s->init_num = length;
			s->init_off = 0;
			return ssl3_do_write(s, SSL3_RT_HANDSHAKE);
			}
		}

	/* no supp data message sent */
	*skip = 1;
	s->init_num = 0;
	s->init_off = 0;
	return 1;

	f_err:
	ssl3_send_alert(s,SSL3_AL_FATAL,al);
	return 0;
	}
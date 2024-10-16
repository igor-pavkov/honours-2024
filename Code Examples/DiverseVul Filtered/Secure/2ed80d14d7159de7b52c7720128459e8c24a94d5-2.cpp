int ssl_parse_clienthello_tlsext(SSL *s, unsigned char **p, unsigned char *d, int n, int *al)
	{
	unsigned short type;
	unsigned short size;
	unsigned short len;
	unsigned char *data = *p;
	int renegotiate_seen = 0;

	s->servername_done = 0;
	s->tlsext_status_type = -1;

#ifndef OPENSSL_NO_EC
	if (s->options & SSL_OP_SAFARI_ECDHE_ECDSA_BUG)
		ssl_check_for_safari(s, data, d, n);
#endif /* !OPENSSL_NO_EC */

	if (data >= (d+n-2))
		goto ri_check;

	n2s(data,len);

	if (data > (d+n-len)) 
		goto ri_check;

	while (data <= (d+n-4))
		{
		n2s(data,type);
		n2s(data,size);

		if (data+size > (d+n))
	   		goto ri_check;

		if (s->tlsext_debug_cb)
			s->tlsext_debug_cb(s, 0, type, data, size,
						s->tlsext_debug_arg);
/* The servername extension is treated as follows:

   - Only the hostname type is supported with a maximum length of 255.
   - The servername is rejected if too long or if it contains zeros,
     in which case an fatal alert is generated.
   - The servername field is maintained together with the session cache.
   - When a session is resumed, the servername call back invoked in order
     to allow the application to position itself to the right context. 
   - The servername is acknowledged if it is new for a session or when 
     it is identical to a previously used for the same session. 
     Applications can control the behaviour.  They can at any time
     set a 'desirable' servername for a new SSL object. This can be the
     case for example with HTTPS when a Host: header field is received and
     a renegotiation is requested. In this case, a possible servername
     presented in the new client hello is only acknowledged if it matches
     the value of the Host: field. 
   - Applications must  use SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION
     if they provide for changing an explicit servername context for the session,
     i.e. when the session has been established with a servername extension. 
   - On session reconnect, the servername extension may be absent. 

*/      

		if (type == TLSEXT_TYPE_server_name)
			{
			unsigned char *sdata;
			int servname_type;
			int dsize; 
		
			if (size < 2) 
				{
				*al = SSL_AD_DECODE_ERROR;
				return 0;
				}
			n2s(data,dsize);  
			size -= 2;
			if (dsize > size  ) 
				{
				*al = SSL_AD_DECODE_ERROR;
				return 0;
				} 

			sdata = data;
			while (dsize > 3) 
				{
	 			servname_type = *(sdata++); 
				n2s(sdata,len);
				dsize -= 3;

				if (len > dsize) 
					{
					*al = SSL_AD_DECODE_ERROR;
					return 0;
					}
				if (s->servername_done == 0)
				switch (servname_type)
					{
				case TLSEXT_NAMETYPE_host_name:
					if (!s->hit)
						{
						if(s->session->tlsext_hostname)
							{
							*al = SSL_AD_DECODE_ERROR;
							return 0;
							}
						if (len > TLSEXT_MAXLEN_host_name)
							{
							*al = TLS1_AD_UNRECOGNIZED_NAME;
							return 0;
							}
						if ((s->session->tlsext_hostname = OPENSSL_malloc(len+1)) == NULL)
							{
							*al = TLS1_AD_INTERNAL_ERROR;
							return 0;
							}
						memcpy(s->session->tlsext_hostname, sdata, len);
						s->session->tlsext_hostname[len]='\0';
						if (strlen(s->session->tlsext_hostname) != len) {
							OPENSSL_free(s->session->tlsext_hostname);
							s->session->tlsext_hostname = NULL;
							*al = TLS1_AD_UNRECOGNIZED_NAME;
							return 0;
						}
						s->servername_done = 1; 

						}
					else 
						s->servername_done = s->session->tlsext_hostname
							&& strlen(s->session->tlsext_hostname) == len 
							&& strncmp(s->session->tlsext_hostname, (char *)sdata, len) == 0;
					
					break;

				default:
					break;
					}
				 
				dsize -= len;
				}
			if (dsize != 0) 
				{
				*al = SSL_AD_DECODE_ERROR;
				return 0;
				}

			}
		else if (type == TLSEXT_TYPE_renegotiate)
			{
			if(!ssl_parse_clienthello_renegotiate_ext(s, data, size, al))
				return 0;
			renegotiate_seen = 1;
			}
		else if (type == TLSEXT_TYPE_status_request &&
		         s->version != DTLS1_VERSION && s->ctx->tlsext_status_cb)
			{
		
			if (size < 5) 
				{
				*al = SSL_AD_DECODE_ERROR;
				return 0;
				}

			s->tlsext_status_type = *data++;
			size--;
			if (s->tlsext_status_type == TLSEXT_STATUSTYPE_ocsp)
				{
				const unsigned char *sdata;
				int dsize;
				/* Read in responder_id_list */
				n2s(data,dsize);
				size -= 2;
				if (dsize > size  ) 
					{
					*al = SSL_AD_DECODE_ERROR;
					return 0;
					}
				while (dsize > 0)
					{
					OCSP_RESPID *id;
					int idsize;
					if (dsize < 4)
						{
						*al = SSL_AD_DECODE_ERROR;
						return 0;
						}
					n2s(data, idsize);
					dsize -= 2 + idsize;
					size -= 2 + idsize;
					if (dsize < 0)
						{
						*al = SSL_AD_DECODE_ERROR;
						return 0;
						}
					sdata = data;
					data += idsize;
					id = d2i_OCSP_RESPID(NULL,
								&sdata, idsize);
					if (!id)
						{
						*al = SSL_AD_DECODE_ERROR;
						return 0;
						}
					if (data != sdata)
						{
						OCSP_RESPID_free(id);
						*al = SSL_AD_DECODE_ERROR;
						return 0;
						}
					if (!s->tlsext_ocsp_ids
						&& !(s->tlsext_ocsp_ids =
						sk_OCSP_RESPID_new_null()))
						{
						OCSP_RESPID_free(id);
						*al = SSL_AD_INTERNAL_ERROR;
						return 0;
						}
					if (!sk_OCSP_RESPID_push(
							s->tlsext_ocsp_ids, id))
						{
						OCSP_RESPID_free(id);
						*al = SSL_AD_INTERNAL_ERROR;
						return 0;
						}
					}

				/* Read in request_extensions */
				if (size < 2)
					{
					*al = SSL_AD_DECODE_ERROR;
					return 0;
					}
				n2s(data,dsize);
				size -= 2;
				if (dsize != size)
					{
					*al = SSL_AD_DECODE_ERROR;
					return 0;
					}
				sdata = data;
				if (dsize > 0)
					{
					if (s->tlsext_ocsp_exts)
						{
						sk_X509_EXTENSION_pop_free(s->tlsext_ocsp_exts,
									   X509_EXTENSION_free);
						}

					s->tlsext_ocsp_exts =
						d2i_X509_EXTENSIONS(NULL,
							&sdata, dsize);
					if (!s->tlsext_ocsp_exts
						|| (data + dsize != sdata))
						{
						*al = SSL_AD_DECODE_ERROR;
						return 0;
						}
					}
				}
				/* We don't know what to do with any other type
 			 	* so ignore it.
 			 	*/
				else
					s->tlsext_status_type = -1;
			}

		/* session ticket processed earlier */

		data+=size;		
		}
	*p = data;

	ri_check:

	/* Need RI if renegotiating */

	if (!renegotiate_seen && s->new_session &&
		!(s->options & SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION))
		{
		*al = SSL_AD_HANDSHAKE_FAILURE;
	 	SSLerr(SSL_F_SSL_PARSE_CLIENTHELLO_TLSEXT,
				SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED);
		return 0;
		}

	return 1;
	}
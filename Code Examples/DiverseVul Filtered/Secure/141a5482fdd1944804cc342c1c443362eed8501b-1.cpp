int ssl3_send_client_key_exchange(SSL *s)
	{
	unsigned char *p,*d;
	int n;
	unsigned long l;
#ifndef OPENSSL_NO_RSA
	unsigned char *q;
	EVP_PKEY *pkey=NULL;
#endif
#ifndef OPENSSL_NO_KRB5
	KSSL_ERR kssl_err;
#endif /* OPENSSL_NO_KRB5 */
#ifndef OPENSSL_NO_ECDH
	EC_KEY *clnt_ecdh = NULL;
	const EC_POINT *srvr_ecpoint = NULL;
	EVP_PKEY *srvr_pub_pkey = NULL;
	unsigned char *encodedPoint = NULL;
	int encoded_pt_len = 0;
	BN_CTX * bn_ctx = NULL;
#endif

	if (s->state == SSL3_ST_CW_KEY_EXCH_A)
		{
		d=(unsigned char *)s->init_buf->data;
		p= &(d[4]);

		l=s->s3->tmp.new_cipher->algorithms;

		/* Fool emacs indentation */
		if (0) {}
#ifndef OPENSSL_NO_RSA
		else if (l & SSL_kRSA)
			{
			RSA *rsa;
			unsigned char tmp_buf[SSL_MAX_MASTER_KEY_LENGTH];

			if (s->session->sess_cert->peer_rsa_tmp != NULL)
				rsa=s->session->sess_cert->peer_rsa_tmp;
			else
				{
				pkey=X509_get_pubkey(s->session->sess_cert->peer_pkeys[SSL_PKEY_RSA_ENC].x509);
				if ((pkey == NULL) ||
					(pkey->type != EVP_PKEY_RSA) ||
					(pkey->pkey.rsa == NULL))
					{
					SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_INTERNAL_ERROR);
					goto err;
					}
				rsa=pkey->pkey.rsa;
				EVP_PKEY_free(pkey);
				}
				
			tmp_buf[0]=s->client_version>>8;
			tmp_buf[1]=s->client_version&0xff;
			if (RAND_bytes(&(tmp_buf[2]),sizeof tmp_buf-2) <= 0)
					goto err;

			s->session->master_key_length=sizeof tmp_buf;

			q=p;
			/* Fix buf for TLS and beyond */
			if (s->version > SSL3_VERSION)
				p+=2;
			n=RSA_public_encrypt(sizeof tmp_buf,
				tmp_buf,p,rsa,RSA_PKCS1_PADDING);
#ifdef PKCS1_CHECK
			if (s->options & SSL_OP_PKCS1_CHECK_1) p[1]++;
			if (s->options & SSL_OP_PKCS1_CHECK_2) tmp_buf[0]=0x70;
#endif
			if (n <= 0)
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,SSL_R_BAD_RSA_ENCRYPT);
				goto err;
				}

			/* Fix buf for TLS and beyond */
			if (s->version > SSL3_VERSION)
				{
				s2n(n,q);
				n+=2;
				}

			s->session->master_key_length=
				s->method->ssl3_enc->generate_master_secret(s,
					s->session->master_key,
					tmp_buf,sizeof tmp_buf);
			OPENSSL_cleanse(tmp_buf,sizeof tmp_buf);
			}
#endif
#ifndef OPENSSL_NO_KRB5
		else if (l & SSL_kKRB5)
			{
			krb5_error_code	krb5rc;
			KSSL_CTX	*kssl_ctx = s->kssl_ctx;
			/*  krb5_data	krb5_ap_req;  */
			krb5_data	*enc_ticket;
			krb5_data	authenticator, *authp = NULL;
			EVP_CIPHER_CTX	ciph_ctx;
			EVP_CIPHER	*enc = NULL;
			unsigned char	iv[EVP_MAX_IV_LENGTH];
			unsigned char	tmp_buf[SSL_MAX_MASTER_KEY_LENGTH];
			unsigned char	epms[SSL_MAX_MASTER_KEY_LENGTH 
						+ EVP_MAX_IV_LENGTH];
			int 		padl, outl = sizeof(epms);

			EVP_CIPHER_CTX_init(&ciph_ctx);

#ifdef KSSL_DEBUG
			printf("ssl3_send_client_key_exchange(%lx & %lx)\n",
			        l, SSL_kKRB5);
#endif	/* KSSL_DEBUG */

			authp = NULL;
#ifdef KRB5SENDAUTH
			if (KRB5SENDAUTH)  authp = &authenticator;
#endif	/* KRB5SENDAUTH */

			krb5rc = kssl_cget_tkt(kssl_ctx, &enc_ticket, authp,
				&kssl_err);
			enc = kssl_map_enc(kssl_ctx->enctype);
			if (enc == NULL)
			    goto err;
#ifdef KSSL_DEBUG
			{
			printf("kssl_cget_tkt rtn %d\n", krb5rc);
			if (krb5rc && kssl_err.text)
			  printf("kssl_cget_tkt kssl_err=%s\n", kssl_err.text);
			}
#endif	/* KSSL_DEBUG */

			if (krb5rc)
				{
				ssl3_send_alert(s,SSL3_AL_FATAL,
						SSL_AD_HANDSHAKE_FAILURE);
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
						kssl_err.reason);
				goto err;
				}

			/*  20010406 VRS - Earlier versions used KRB5 AP_REQ
			**  in place of RFC 2712 KerberosWrapper, as in:
			**
			**  Send ticket (copy to *p, set n = length)
			**  n = krb5_ap_req.length;
			**  memcpy(p, krb5_ap_req.data, krb5_ap_req.length);
			**  if (krb5_ap_req.data)  
			**    kssl_krb5_free_data_contents(NULL,&krb5_ap_req);
			**
			**  Now using real RFC 2712 KerberosWrapper
			**  (Thanks to Simon Wilkinson <sxw@sxw.org.uk>)
			**  Note: 2712 "opaque" types are here replaced
			**  with a 2-byte length followed by the value.
			**  Example:
			**  KerberosWrapper= xx xx asn1ticket 0 0 xx xx encpms
			**  Where "xx xx" = length bytes.  Shown here with
			**  optional authenticator omitted.
			*/

			/*  KerberosWrapper.Ticket		*/
			s2n(enc_ticket->length,p);
			memcpy(p, enc_ticket->data, enc_ticket->length);
			p+= enc_ticket->length;
			n = enc_ticket->length + 2;

			/*  KerberosWrapper.Authenticator	*/
			if (authp  &&  authp->length)  
				{
				s2n(authp->length,p);
				memcpy(p, authp->data, authp->length);
				p+= authp->length;
				n+= authp->length + 2;
				
				free(authp->data);
				authp->data = NULL;
				authp->length = 0;
				}
			else
				{
				s2n(0,p);/*  null authenticator length	*/
				n+=2;
				}
 
			    tmp_buf[0]=s->client_version>>8;
			    tmp_buf[1]=s->client_version&0xff;
			    if (RAND_bytes(&(tmp_buf[2]),sizeof tmp_buf-2) <= 0)
				goto err;

			/*  20010420 VRS.  Tried it this way; failed.
			**	EVP_EncryptInit_ex(&ciph_ctx,enc, NULL,NULL);
			**	EVP_CIPHER_CTX_set_key_length(&ciph_ctx,
			**				kssl_ctx->length);
			**	EVP_EncryptInit_ex(&ciph_ctx,NULL, key,iv);
			*/

			memset(iv, 0, sizeof iv);  /* per RFC 1510 */
			EVP_EncryptInit_ex(&ciph_ctx,enc, NULL,
				kssl_ctx->key,iv);
			EVP_EncryptUpdate(&ciph_ctx,epms,&outl,tmp_buf,
				sizeof tmp_buf);
			EVP_EncryptFinal_ex(&ciph_ctx,&(epms[outl]),&padl);
			outl += padl;
			if (outl > sizeof epms)
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_INTERNAL_ERROR);
				goto err;
				}
			EVP_CIPHER_CTX_cleanup(&ciph_ctx);

			/*  KerberosWrapper.EncryptedPreMasterSecret	*/
			s2n(outl,p);
			memcpy(p, epms, outl);
			p+=outl;
			n+=outl + 2;

			s->session->master_key_length=
			        s->method->ssl3_enc->generate_master_secret(s,
					s->session->master_key,
					tmp_buf, sizeof tmp_buf);

			OPENSSL_cleanse(tmp_buf, sizeof tmp_buf);
			OPENSSL_cleanse(epms, outl);
			}
#endif
#ifndef OPENSSL_NO_DH
		else if (l & (SSL_kEDH|SSL_kDHr|SSL_kDHd))
			{
			DH *dh_srvr,*dh_clnt;

			if (s->session->sess_cert == NULL) 
				{
				ssl3_send_alert(s,SSL3_AL_FATAL,SSL_AD_UNEXPECTED_MESSAGE);
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,SSL_R_UNEXPECTED_MESSAGE);
				goto err;
			        }

			if (s->session->sess_cert->peer_dh_tmp != NULL)
				dh_srvr=s->session->sess_cert->peer_dh_tmp;
			else
				{
				/* we get them from the cert */
				ssl3_send_alert(s,SSL3_AL_FATAL,SSL_AD_HANDSHAKE_FAILURE);
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,SSL_R_UNABLE_TO_FIND_DH_PARAMETERS);
				goto err;
				}
			
			/* generate a new random key */
			if ((dh_clnt=DHparams_dup(dh_srvr)) == NULL)
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_DH_LIB);
				goto err;
				}
			if (!DH_generate_key(dh_clnt))
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_DH_LIB);
				goto err;
				}

			/* use the 'p' output buffer for the DH key, but
			 * make sure to clear it out afterwards */

			n=DH_compute_key(p,dh_srvr->pub_key,dh_clnt);

			if (n <= 0)
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_DH_LIB);
				goto err;
				}

			/* generate master key from the result */
			s->session->master_key_length=
				s->method->ssl3_enc->generate_master_secret(s,
					s->session->master_key,p,n);
			/* clean up */
			memset(p,0,n);

			/* send off the data */
			n=BN_num_bytes(dh_clnt->pub_key);
			s2n(n,p);
			BN_bn2bin(dh_clnt->pub_key,p);
			n+=2;

			DH_free(dh_clnt);

			/* perhaps clean things up a bit EAY EAY EAY EAY*/
			}
#endif

#ifndef OPENSSL_NO_ECDH 
		else if ((l & SSL_kECDH) || (l & SSL_kECDHE))
			{
			const EC_GROUP *srvr_group = NULL;
			EC_KEY *tkey;
			int ecdh_clnt_cert = 0;
			int field_size = 0;

			if (s->session->sess_cert == NULL) 
				{
				ssl3_send_alert(s,SSL3_AL_FATAL,SSL_AD_UNEXPECTED_MESSAGE);
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,SSL_R_UNEXPECTED_MESSAGE);
				goto err;
				}

			/* Did we send out the client's
			 * ECDH share for use in premaster
			 * computation as part of client certificate?
			 * If so, set ecdh_clnt_cert to 1.
			 */
			if ((l & SSL_kECDH) && (s->cert != NULL)) 
				{
				/* XXX: For now, we do not support client
				 * authentication using ECDH certificates.
				 * To add such support, one needs to add
				 * code that checks for appropriate 
				 * conditions and sets ecdh_clnt_cert to 1.
				 * For example, the cert have an ECC
				 * key on the same curve as the server's
				 * and the key should be authorized for
				 * key agreement.
				 *
				 * One also needs to add code in ssl3_connect
				 * to skip sending the certificate verify
				 * message.
				 *
				 * if ((s->cert->key->privatekey != NULL) &&
				 *     (s->cert->key->privatekey->type ==
				 *      EVP_PKEY_EC) && ...)
				 * ecdh_clnt_cert = 1;
				 */
				}

			if (s->session->sess_cert->peer_ecdh_tmp != NULL)
				{
				tkey = s->session->sess_cert->peer_ecdh_tmp;
				}
			else
				{
				/* Get the Server Public Key from Cert */
				srvr_pub_pkey = X509_get_pubkey(s->session-> \
				    sess_cert->peer_pkeys[SSL_PKEY_ECC].x509);
				if ((srvr_pub_pkey == NULL) ||
				    (srvr_pub_pkey->type != EVP_PKEY_EC) ||
				    (srvr_pub_pkey->pkey.ec == NULL))
					{
					SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
					    ERR_R_INTERNAL_ERROR);
					goto err;
					}

				tkey = srvr_pub_pkey->pkey.ec;
				}

			srvr_group   = EC_KEY_get0_group(tkey);
			srvr_ecpoint = EC_KEY_get0_public_key(tkey);

			if ((srvr_group == NULL) || (srvr_ecpoint == NULL))
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
				    ERR_R_INTERNAL_ERROR);
				goto err;
				}

			if ((clnt_ecdh=EC_KEY_new()) == NULL) 
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_MALLOC_FAILURE);
				goto err;
				}

			if (!EC_KEY_set_group(clnt_ecdh, srvr_group))
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_EC_LIB);
				goto err;
				}
			if (ecdh_clnt_cert) 
				{ 
				/* Reuse key info from our certificate
				 * We only need our private key to perform
				 * the ECDH computation.
				 */
				const BIGNUM *priv_key;
				tkey = s->cert->key->privatekey->pkey.ec;
				priv_key = EC_KEY_get0_private_key(tkey);
				if (priv_key == NULL)
					{
					SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_MALLOC_FAILURE);
					goto err;
					}
				if (!EC_KEY_set_private_key(clnt_ecdh, priv_key))
					{
					SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_EC_LIB);
					goto err;
					}
				}
			else 
				{
				/* Generate a new ECDH key pair */
				if (!(EC_KEY_generate_key(clnt_ecdh)))
					{
					SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, ERR_R_ECDH_LIB);
					goto err;
					}
				}

			/* use the 'p' output buffer for the ECDH key, but
			 * make sure to clear it out afterwards
			 */

			field_size = EC_GROUP_get_degree(srvr_group);
			if (field_size <= 0)
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, 
				       ERR_R_ECDH_LIB);
				goto err;
				}
			n=ECDH_compute_key(p, (field_size+7)/8, srvr_ecpoint, clnt_ecdh, NULL);
			if (n <= 0)
				{
				SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE, 
				       ERR_R_ECDH_LIB);
				goto err;
				}

			/* generate master key from the result */
			s->session->master_key_length = s->method->ssl3_enc \
			    -> generate_master_secret(s, 
				s->session->master_key,
				p, n);

			memset(p, 0, n); /* clean up */

			if (ecdh_clnt_cert) 
				{
				/* Send empty client key exch message */
				n = 0;
				}
			else 
				{
				/* First check the size of encoding and
				 * allocate memory accordingly.
				 */
				encoded_pt_len = 
				    EC_POINT_point2oct(srvr_group, 
					EC_KEY_get0_public_key(clnt_ecdh), 
					POINT_CONVERSION_UNCOMPRESSED, 
					NULL, 0, NULL);

				encodedPoint = (unsigned char *) 
				    OPENSSL_malloc(encoded_pt_len * 
					sizeof(unsigned char)); 
				bn_ctx = BN_CTX_new();
				if ((encodedPoint == NULL) || 
				    (bn_ctx == NULL)) 
					{
					SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,ERR_R_MALLOC_FAILURE);
					goto err;
					}

				/* Encode the public key */
				n = EC_POINT_point2oct(srvr_group, 
				    EC_KEY_get0_public_key(clnt_ecdh), 
				    POINT_CONVERSION_UNCOMPRESSED, 
				    encodedPoint, encoded_pt_len, bn_ctx);

				*p = n; /* length of encoded point */
				/* Encoded point will be copied here */
				p += 1; 
				/* copy the point */
				memcpy((unsigned char *)p, encodedPoint, n);
				/* increment n to account for length field */
				n += 1; 
				}

			/* Free allocated memory */
			BN_CTX_free(bn_ctx);
			if (encodedPoint != NULL) OPENSSL_free(encodedPoint);
			if (clnt_ecdh != NULL) 
				 EC_KEY_free(clnt_ecdh);
			EVP_PKEY_free(srvr_pub_pkey);
			}
#endif /* !OPENSSL_NO_ECDH */
		else
			{
			ssl3_send_alert(s, SSL3_AL_FATAL,
			    SSL_AD_HANDSHAKE_FAILURE);
			SSLerr(SSL_F_SSL3_SEND_CLIENT_KEY_EXCHANGE,
			    ERR_R_INTERNAL_ERROR);
			goto err;
			}
		
		*(d++)=SSL3_MT_CLIENT_KEY_EXCHANGE;
		l2n3(n,d);

		s->state=SSL3_ST_CW_KEY_EXCH_B;
		/* number of bytes to write */
		s->init_num=n+4;
		s->init_off=0;
		}

	/* SSL3_ST_CW_KEY_EXCH_B */
	return(ssl3_do_write(s,SSL3_RT_HANDSHAKE));
err:
#ifndef OPENSSL_NO_ECDH
	BN_CTX_free(bn_ctx);
	if (encodedPoint != NULL) OPENSSL_free(encodedPoint);
	if (clnt_ecdh != NULL) 
		EC_KEY_free(clnt_ecdh);
	EVP_PKEY_free(srvr_pub_pkey);
#endif
	return(-1);
	}
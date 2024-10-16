int ssl3_send_newsession_ticket(SSL *s)
{
    unsigned char *senc = NULL;
    EVP_CIPHER_CTX ctx;
    HMAC_CTX hctx;

    if (s->state == SSL3_ST_SW_SESSION_TICKET_A) {
        unsigned char *p, *macstart;
        const unsigned char *const_p;
        int len, slen_full, slen;
        SSL_SESSION *sess;
        unsigned int hlen;
        SSL_CTX *tctx = s->initial_ctx;
        unsigned char iv[EVP_MAX_IV_LENGTH];
        unsigned char key_name[16];

        /* get session encoding length */
        slen_full = i2d_SSL_SESSION(s->session, NULL);
        /*
         * Some length values are 16 bits, so forget it if session is too
         * long
         */
        if (slen_full == 0 || slen_full > 0xFF00)
            return -1;
        senc = OPENSSL_malloc(slen_full);
        if (!senc)
            return -1;

        EVP_CIPHER_CTX_init(&ctx);
        HMAC_CTX_init(&hctx);

        p = senc;
        if (!i2d_SSL_SESSION(s->session, &p))
            goto err;

        /*
         * create a fresh copy (not shared with other threads) to clean up
         */
        const_p = senc;
        sess = d2i_SSL_SESSION(NULL, &const_p, slen_full);
        if (sess == NULL)
            goto err;
        sess->session_id_length = 0; /* ID is irrelevant for the ticket */

        slen = i2d_SSL_SESSION(sess, NULL);
        if (slen == 0 || slen > slen_full) { /* shouldn't ever happen */
            SSL_SESSION_free(sess);
            goto err;
        }
        p = senc;
        if (!i2d_SSL_SESSION(sess, &p)) {
            SSL_SESSION_free(sess);
            goto err;
        }
        SSL_SESSION_free(sess);

        /*-
         * Grow buffer if need be: the length calculation is as
         * follows handshake_header_length +
         * 4 (ticket lifetime hint) + 2 (ticket length) +
         * 16 (key name) + max_iv_len (iv length) +
         * session_length + max_enc_block_size (max encrypted session
         * length) + max_md_size (HMAC).
         */
        if (!BUF_MEM_grow(s->init_buf,
                          SSL_HM_HEADER_LENGTH(s) + 22 + EVP_MAX_IV_LENGTH +
                          EVP_MAX_BLOCK_LENGTH + EVP_MAX_MD_SIZE + slen))
            goto err;

        p = ssl_handshake_start(s);
        /*
         * Initialize HMAC and cipher contexts. If callback present it does
         * all the work otherwise use generated values from parent ctx.
         */
        if (tctx->tlsext_ticket_key_cb) {
            if (tctx->tlsext_ticket_key_cb(s, key_name, iv, &ctx,
                                           &hctx, 1) < 0)
                goto err;
        } else {
            if (RAND_bytes(iv, 16) <= 0)
                goto err;
            if (!EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL,
                                    tctx->tlsext_tick_aes_key, iv))
                goto err;
            if (!HMAC_Init_ex(&hctx, tctx->tlsext_tick_hmac_key, 16,
                              tlsext_tick_md(), NULL))
                goto err;
            memcpy(key_name, tctx->tlsext_tick_key_name, 16);
        }

        /*
         * Ticket lifetime hint (advisory only): We leave this unspecified
         * for resumed session (for simplicity), and guess that tickets for
         * new sessions will live as long as their sessions.
         */
        l2n(s->hit ? 0 : s->session->timeout, p);

        /* Skip ticket length for now */
        p += 2;
        /* Output key name */
        macstart = p;
        memcpy(p, key_name, 16);
        p += 16;
        /* output IV */
        memcpy(p, iv, EVP_CIPHER_CTX_iv_length(&ctx));
        p += EVP_CIPHER_CTX_iv_length(&ctx);
        /* Encrypt session data */
        if (!EVP_EncryptUpdate(&ctx, p, &len, senc, slen))
            goto err;
        p += len;
        if (!EVP_EncryptFinal(&ctx, p, &len))
            goto err;
        p += len;

        if (!HMAC_Update(&hctx, macstart, p - macstart))
            goto err;
        if (!HMAC_Final(&hctx, p, &hlen))
            goto err;

        EVP_CIPHER_CTX_cleanup(&ctx);
        HMAC_CTX_cleanup(&hctx);

        p += hlen;
        /* Now write out lengths: p points to end of data written */
        /* Total length */
        len = p - ssl_handshake_start(s);
        ssl_set_handshake_header(s, SSL3_MT_NEWSESSION_TICKET, len);
        /* Skip ticket lifetime hint */
        p = ssl_handshake_start(s) + 4;
        s2n(len - 6, p);
        s->state = SSL3_ST_SW_SESSION_TICKET_B;
        OPENSSL_free(senc);
    }

    /* SSL3_ST_SW_SESSION_TICKET_B */
    return ssl_do_write(s);
 err:
    if (senc)
        OPENSSL_free(senc);
    EVP_CIPHER_CTX_cleanup(&ctx);
    HMAC_CTX_cleanup(&hctx);
    return -1;
}
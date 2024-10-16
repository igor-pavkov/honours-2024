PHP_FUNCTION(openssl_pkey_get_details)
{
	zval *key;
	EVP_PKEY *pkey;
	BIO *out;
	unsigned int pbio_len;
	char *pbio;
	zend_long ktype;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "r", &key) == FAILURE) {
		return;
	}
	if ((pkey = (EVP_PKEY *)zend_fetch_resource(Z_RES_P(key), "OpenSSL key", le_key)) == NULL) {
		RETURN_FALSE;
	}
	out = BIO_new(BIO_s_mem());
	if (!PEM_write_bio_PUBKEY(out, pkey)) {
		BIO_free(out);
		php_openssl_store_errors();
		RETURN_FALSE;
	}
	pbio_len = BIO_get_mem_data(out, &pbio);

	array_init(return_value);
	add_assoc_long(return_value, "bits", EVP_PKEY_bits(pkey));
	add_assoc_stringl(return_value, "key", pbio, pbio_len);
	/*TODO: Use the real values once the openssl constants are used
	 * See the enum at the top of this file
	 */
	switch (EVP_PKEY_base_id(pkey)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			{
				RSA *rsa = EVP_PKEY_get0_RSA(pkey);
				ktype = OPENSSL_KEYTYPE_RSA;

				if (rsa != NULL) {
					zval z_rsa;
					const BIGNUM *n, *e, *d, *p, *q, *dmp1, *dmq1, *iqmp;

					RSA_get0_key(rsa, &n, &e, &d);
					RSA_get0_factors(rsa, &p, &q);
					RSA_get0_crt_params(rsa, &dmp1, &dmq1, &iqmp);

					array_init(&z_rsa);
					OPENSSL_PKEY_GET_BN(z_rsa, n);
					OPENSSL_PKEY_GET_BN(z_rsa, e);
					OPENSSL_PKEY_GET_BN(z_rsa, d);
					OPENSSL_PKEY_GET_BN(z_rsa, p);
					OPENSSL_PKEY_GET_BN(z_rsa, q);
					OPENSSL_PKEY_GET_BN(z_rsa, dmp1);
					OPENSSL_PKEY_GET_BN(z_rsa, dmq1);
					OPENSSL_PKEY_GET_BN(z_rsa, iqmp);
					add_assoc_zval(return_value, "rsa", &z_rsa);
				}
			}
			break;
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			{
				DSA *dsa = EVP_PKEY_get0_DSA(pkey);
				ktype = OPENSSL_KEYTYPE_DSA;

				if (dsa != NULL) {
					zval z_dsa;
					const BIGNUM *p, *q, *g, *priv_key, *pub_key;

					DSA_get0_pqg(dsa, &p, &q, &g);
					DSA_get0_key(dsa, &pub_key, &priv_key);

					array_init(&z_dsa);
					OPENSSL_PKEY_GET_BN(z_dsa, p);
					OPENSSL_PKEY_GET_BN(z_dsa, q);
					OPENSSL_PKEY_GET_BN(z_dsa, g);
					OPENSSL_PKEY_GET_BN(z_dsa, priv_key);
					OPENSSL_PKEY_GET_BN(z_dsa, pub_key);
					add_assoc_zval(return_value, "dsa", &z_dsa);
				}
			}
			break;
		case EVP_PKEY_DH:
			{
				DH *dh = EVP_PKEY_get0_DH(pkey);
				ktype = OPENSSL_KEYTYPE_DH;

				if (dh != NULL) {
					zval z_dh;
					const BIGNUM *p, *q, *g, *priv_key, *pub_key;

					DH_get0_pqg(dh, &p, &q, &g);
					DH_get0_key(dh, &pub_key, &priv_key);

					array_init(&z_dh);
					OPENSSL_PKEY_GET_BN(z_dh, p);
					OPENSSL_PKEY_GET_BN(z_dh, g);
					OPENSSL_PKEY_GET_BN(z_dh, priv_key);
					OPENSSL_PKEY_GET_BN(z_dh, pub_key);
					add_assoc_zval(return_value, "dh", &z_dh);
				}
			}
			break;
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			ktype = OPENSSL_KEYTYPE_EC;
			if (EVP_PKEY_get0_EC_KEY(pkey) != NULL) {
				zval ec;
				const EC_GROUP *ec_group;
				const EC_POINT *pub;
				int nid;
				char *crv_sn;
				ASN1_OBJECT *obj;
				// openssl recommends a buffer length of 80
				char oir_buf[80];
				const EC_KEY *ec_key = EVP_PKEY_get0_EC_KEY(pkey);
				BIGNUM *x = BN_new();
				BIGNUM *y = BN_new();
				const BIGNUM *d;

				ec_group = EC_KEY_get0_group(ec_key);

				// Curve nid (numerical identifier) used for ASN1 mapping
				nid = EC_GROUP_get_curve_name(ec_group);
				if (nid == NID_undef) {
					break;
				}
				array_init(&ec);

				// Short object name
				crv_sn = (char*) OBJ_nid2sn(nid);
				if (crv_sn != NULL) {
					add_assoc_string(&ec, "curve_name", crv_sn);
				}

				obj = OBJ_nid2obj(nid);
				if (obj != NULL) {
					int oir_len = OBJ_obj2txt(oir_buf, sizeof(oir_buf), obj, 1);
					add_assoc_stringl(&ec, "curve_oid", (char*) oir_buf, oir_len);
					ASN1_OBJECT_free(obj);
				}

				pub = EC_KEY_get0_public_key(ec_key);

				if (EC_POINT_get_affine_coordinates_GFp(ec_group, pub, x, y, NULL)) {
					OPENSSL_GET_BN(ec, x, x);
					OPENSSL_GET_BN(ec, y, y);
				} else {
					php_openssl_store_errors();
				}

				if ((d = EC_KEY_get0_private_key(EVP_PKEY_get0_EC_KEY(pkey))) != NULL) {
					OPENSSL_GET_BN(ec, d, d);
				}

				add_assoc_zval(return_value, "ec", &ec);

				BN_free(x);
				BN_free(y);
			}
			break;
#endif
		default:
			ktype = -1;
			break;
	}
	add_assoc_long(return_value, "type", ktype);

	BIO_free(out);
}
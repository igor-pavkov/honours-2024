PHP_FUNCTION(openssl_pkey_get_details)
{
	zval *key;
	EVP_PKEY *pkey;
	BIO *out;
	unsigned int pbio_len;
	char *pbio;
	long ktype;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &key) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(pkey, EVP_PKEY *, &key, -1, "OpenSSL key", le_key);
	if (!pkey) {
		RETURN_FALSE;
	}
	out = BIO_new(BIO_s_mem());
	PEM_write_bio_PUBKEY(out, pkey);
	pbio_len = BIO_get_mem_data(out, &pbio);

	array_init(return_value);
	add_assoc_long(return_value, "bits", EVP_PKEY_bits(pkey));
	add_assoc_stringl(return_value, "key", pbio, pbio_len, 1);
	/*TODO: Use the real values once the openssl constants are used 
	 * See the enum at the top of this file
	 */
	switch (EVP_PKEY_type(pkey->type)) {
		case EVP_PKEY_RSA:
		case EVP_PKEY_RSA2:
			ktype = OPENSSL_KEYTYPE_RSA;

			if (pkey->pkey.rsa != NULL) {
				zval *rsa;

				ALLOC_INIT_ZVAL(rsa);
				array_init(rsa);
				OPENSSL_PKEY_GET_BN(rsa, n);
				OPENSSL_PKEY_GET_BN(rsa, e);
				OPENSSL_PKEY_GET_BN(rsa, d);
				OPENSSL_PKEY_GET_BN(rsa, p);
				OPENSSL_PKEY_GET_BN(rsa, q);
				OPENSSL_PKEY_GET_BN(rsa, dmp1);
				OPENSSL_PKEY_GET_BN(rsa, dmq1);
				OPENSSL_PKEY_GET_BN(rsa, iqmp);
				add_assoc_zval(return_value, "rsa", rsa);
			}

			break;	
		case EVP_PKEY_DSA:
		case EVP_PKEY_DSA2:
		case EVP_PKEY_DSA3:
		case EVP_PKEY_DSA4:
			ktype = OPENSSL_KEYTYPE_DSA;

			if (pkey->pkey.dsa != NULL) {
				zval *dsa;

				ALLOC_INIT_ZVAL(dsa);
				array_init(dsa);
				OPENSSL_PKEY_GET_BN(dsa, p);
				OPENSSL_PKEY_GET_BN(dsa, q);
				OPENSSL_PKEY_GET_BN(dsa, g);
				OPENSSL_PKEY_GET_BN(dsa, priv_key);
				OPENSSL_PKEY_GET_BN(dsa, pub_key);
				add_assoc_zval(return_value, "dsa", dsa);
			}
			break;
		case EVP_PKEY_DH:
			
			ktype = OPENSSL_KEYTYPE_DH;

			if (pkey->pkey.dh != NULL) {
				zval *dh;

				ALLOC_INIT_ZVAL(dh);
				array_init(dh);
				OPENSSL_PKEY_GET_BN(dh, p);
				OPENSSL_PKEY_GET_BN(dh, g);
				OPENSSL_PKEY_GET_BN(dh, priv_key);
				OPENSSL_PKEY_GET_BN(dh, pub_key);
				add_assoc_zval(return_value, "dh", dh);
			}

			break;
#ifdef HAVE_EVP_PKEY_EC
		case EVP_PKEY_EC:
			ktype = OPENSSL_KEYTYPE_EC;
			if (pkey->pkey.ec != NULL) {
				zval *ec;
				const EC_GROUP *ec_group;
				int nid;
				char *crv_sn;
				ASN1_OBJECT *obj;
				// openssl recommends a buffer length of 80
				char oir_buf[80];

				ec_group = EC_KEY_get0_group(EVP_PKEY_get1_EC_KEY(pkey));

				// Curve nid (numerical identifier) used for ASN1 mapping
				nid = EC_GROUP_get_curve_name(ec_group);
				if (nid == NID_undef) {
					break;
				}
				ALLOC_INIT_ZVAL(ec);
				array_init(ec);

				// Short object name
				crv_sn = (char*) OBJ_nid2sn(nid);
				if (crv_sn != NULL) {
					add_assoc_string(ec, "curve_name", crv_sn, 1);
				}

				obj = OBJ_nid2obj(nid);
				if (obj != NULL) {
					int oir_len = OBJ_obj2txt(oir_buf, sizeof(oir_buf), obj, 1);
					add_assoc_stringl(ec, "curve_oid", (char*)oir_buf, oir_len, 1);
					ASN1_OBJECT_free(obj);
				}

				add_assoc_zval(return_value, "ec", ec);
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
PHP_FUNCTION(openssl_x509_parse)
{
	zval * zcert;
	X509 * cert = NULL;
	int i, sig_nid;
	zend_bool useshortnames = 1;
	char * tmpstr;
	zval subitem;
	X509_EXTENSION *extension;
	X509_NAME *subject_name;
	char *cert_name;
	char *extname;
	BIO *bio_out;
	BUF_MEM *bio_buf;
	ASN1_INTEGER *asn1_serial;
	BIGNUM *bn_serial;
	char *str_serial;
	char *hex_serial;
	char buf[256];

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|b", &zcert, &useshortnames) == FAILURE) {
		return;
	}
	cert = php_openssl_x509_from_zval(zcert, 0, NULL);
	if (cert == NULL) {
		RETURN_FALSE;
	}
	array_init(return_value);

	subject_name = X509_get_subject_name(cert);
	cert_name = X509_NAME_oneline(subject_name, NULL, 0);
	add_assoc_string(return_value, "name", cert_name);
	OPENSSL_free(cert_name);

	php_openssl_add_assoc_name_entry(return_value, "subject", subject_name, useshortnames);
	/* hash as used in CA directories to lookup cert by subject name */
	{
		char buf[32];
		snprintf(buf, sizeof(buf), "%08lx", X509_subject_name_hash(cert));
		add_assoc_string(return_value, "hash", buf);
	}

	php_openssl_add_assoc_name_entry(return_value, "issuer", X509_get_issuer_name(cert), useshortnames);
	add_assoc_long(return_value, "version", X509_get_version(cert));

	asn1_serial = X509_get_serialNumber(cert);

	bn_serial = ASN1_INTEGER_to_BN(asn1_serial, NULL);
	/* Can return NULL on error or memory allocation failure */
	if (!bn_serial) {
		php_openssl_store_errors();
		RETURN_FALSE;
	}

	hex_serial = BN_bn2hex(bn_serial);
	BN_free(bn_serial);
	/* Can return NULL on error or memory allocation failure */
	if (!hex_serial) {
		php_openssl_store_errors();
		RETURN_FALSE;
	}

	str_serial = i2s_ASN1_INTEGER(NULL, asn1_serial);
	add_assoc_string(return_value, "serialNumber", str_serial);
	OPENSSL_free(str_serial);

	/* Return the hex representation of the serial number, as defined by OpenSSL */
	add_assoc_string(return_value, "serialNumberHex", hex_serial);
	OPENSSL_free(hex_serial);

	php_openssl_add_assoc_asn1_string(return_value, "validFrom", 	X509_get_notBefore(cert));
	php_openssl_add_assoc_asn1_string(return_value, "validTo", 		X509_get_notAfter(cert));

	add_assoc_long(return_value, "validFrom_time_t", php_openssl_asn1_time_to_time_t(X509_get_notBefore(cert)));
	add_assoc_long(return_value, "validTo_time_t",  php_openssl_asn1_time_to_time_t(X509_get_notAfter(cert)));

	tmpstr = (char *)X509_alias_get0(cert, NULL);
	if (tmpstr) {
		add_assoc_string(return_value, "alias", tmpstr);
	}

	sig_nid = X509_get_signature_nid(cert);
	add_assoc_string(return_value, "signatureTypeSN", (char*)OBJ_nid2sn(sig_nid));
	add_assoc_string(return_value, "signatureTypeLN", (char*)OBJ_nid2ln(sig_nid));
	add_assoc_long(return_value, "signatureTypeNID", sig_nid);
	array_init(&subitem);

	/* NOTE: the purposes are added as integer keys - the keys match up to the X509_PURPOSE_SSL_XXX defines
	   in x509v3.h */
	for (i = 0; i < X509_PURPOSE_get_count(); i++) {
		int id, purpset;
		char * pname;
		X509_PURPOSE * purp;
		zval subsub;

		array_init(&subsub);

		purp = X509_PURPOSE_get0(i);
		id = X509_PURPOSE_get_id(purp);

		purpset = X509_check_purpose(cert, id, 0);
		add_index_bool(&subsub, 0, purpset);

		purpset = X509_check_purpose(cert, id, 1);
		add_index_bool(&subsub, 1, purpset);

		pname = useshortnames ? X509_PURPOSE_get0_sname(purp) : X509_PURPOSE_get0_name(purp);
		add_index_string(&subsub, 2, pname);

		/* NOTE: if purpset > 1 then it's a warning - we should mention it ? */

		add_index_zval(&subitem, id, &subsub);
	}
	add_assoc_zval(return_value, "purposes", &subitem);

	array_init(&subitem);


	for (i = 0; i < X509_get_ext_count(cert); i++) {
		int nid;
		extension = X509_get_ext(cert, i);
		nid = OBJ_obj2nid(X509_EXTENSION_get_object(extension));
		if (nid != NID_undef) {
			extname = (char *)OBJ_nid2sn(OBJ_obj2nid(X509_EXTENSION_get_object(extension)));
		} else {
			OBJ_obj2txt(buf, sizeof(buf)-1, X509_EXTENSION_get_object(extension), 1);
			extname = buf;
		}
		bio_out = BIO_new(BIO_s_mem());
		if (bio_out == NULL) {
			php_openssl_store_errors();
			RETURN_FALSE;
		}
		if (nid == NID_subject_alt_name) {
			if (openssl_x509v3_subjectAltName(bio_out, extension) == 0) {
				BIO_get_mem_ptr(bio_out, &bio_buf);
				add_assoc_stringl(&subitem, extname, bio_buf->data, bio_buf->length);
			} else {
				zval_dtor(return_value);
				BIO_free(bio_out);
				if (Z_TYPE_P(zcert) != IS_RESOURCE) {
					X509_free(cert);
				}
				RETURN_FALSE;
			}
		}
		else if (X509V3_EXT_print(bio_out, extension, 0, 0)) {
			BIO_get_mem_ptr(bio_out, &bio_buf);
			add_assoc_stringl(&subitem, extname, bio_buf->data, bio_buf->length);
		} else {
			php_openssl_add_assoc_asn1_string(&subitem, extname, X509_EXTENSION_get_data(extension));
		}
		BIO_free(bio_out);
	}
	add_assoc_zval(return_value, "extensions", &subitem);
	if (Z_TYPE_P(zcert) != IS_RESOURCE) {
		X509_free(cert);
	}
}
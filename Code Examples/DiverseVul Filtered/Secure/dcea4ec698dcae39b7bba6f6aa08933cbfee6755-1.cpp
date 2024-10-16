static int openssl_x509v3_subjectAltName(BIO *bio, X509_EXTENSION *extension)
{
	GENERAL_NAMES *names;
	const X509V3_EXT_METHOD *method = NULL;
	long i, length, num;
	const unsigned char *p;

	method = X509V3_EXT_get(extension);
	if (method == NULL) {
		return -1;
	}

	p = extension->value->data;
	length = extension->value->length;
	if (method->it) {
		names = (GENERAL_NAMES*)(ASN1_item_d2i(NULL, &p, length,
						       ASN1_ITEM_ptr(method->it)));
	} else {
		names = (GENERAL_NAMES*)(method->d2i(NULL, &p, length));
	}
	if (names == NULL) {
		return -1;
	}

	num = sk_GENERAL_NAME_num(names);
	for (i = 0; i < num; i++) {
			GENERAL_NAME *name;
			ASN1_STRING *as;
			name = sk_GENERAL_NAME_value(names, i);
			switch (name->type) {
				case GEN_EMAIL:
					BIO_puts(bio, "email:");
					as = name->d.rfc822Name;
					BIO_write(bio, ASN1_STRING_data(as),
						  ASN1_STRING_length(as));
					break;
				case GEN_DNS:
					BIO_puts(bio, "DNS:");
					as = name->d.dNSName;
					BIO_write(bio, ASN1_STRING_data(as),
						  ASN1_STRING_length(as));
					break;
				case GEN_URI:
					BIO_puts(bio, "URI:");
					as = name->d.uniformResourceIdentifier;
					BIO_write(bio, ASN1_STRING_data(as),
						  ASN1_STRING_length(as));
					break;
				default:
					/* use builtin print for GEN_OTHERNAME, GEN_X400,
					 * GEN_EDIPARTY, GEN_DIRNAME, GEN_IPADD and GEN_RID
					 */
					GENERAL_NAME_print(bio, name);
			}
			/* trailing ', ' except for last element */
			if (i < (num - 1)) {
				BIO_puts(bio, ", ");
			}
	}
	sk_GENERAL_NAME_pop_free(names, GENERAL_NAME_free);

	return 0;
}
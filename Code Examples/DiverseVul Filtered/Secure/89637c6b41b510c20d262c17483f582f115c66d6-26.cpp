static void add_assoc_name_entry(zval * val, char * key, X509_NAME * name, int shortname TSRMLS_DC) /* {{{ */
{
	zval **data;
	zval *subitem, *subentries;
	int i;
	char *sname;
	int nid;
	X509_NAME_ENTRY * ne;
	ASN1_STRING * str = NULL;
	ASN1_OBJECT * obj;

	if (key != NULL) {
		MAKE_STD_ZVAL(subitem);
		array_init(subitem);
	} else {
		subitem = val;
	}
	
	for (i = 0; i < X509_NAME_entry_count(name); i++) {
		unsigned char *to_add;
		int to_add_len = 0;


		ne  = X509_NAME_get_entry(name, i);
		obj = X509_NAME_ENTRY_get_object(ne);
		nid = OBJ_obj2nid(obj);

		if (shortname) {
			sname = (char *) OBJ_nid2sn(nid);
		} else {
			sname = (char *) OBJ_nid2ln(nid);
		}

		str = X509_NAME_ENTRY_get_data(ne);
		if (ASN1_STRING_type(str) != V_ASN1_UTF8STRING) {
			to_add_len = ASN1_STRING_to_UTF8(&to_add, str);
		} else {
			to_add = ASN1_STRING_data(str);
			to_add_len = ASN1_STRING_length(str);
		}

		if (to_add_len != -1) {
			if (zend_hash_find(Z_ARRVAL_P(subitem), sname, strlen(sname)+1, (void**)&data) == SUCCESS) {
				if (Z_TYPE_PP(data) == IS_ARRAY) {
					subentries = *data;
					add_next_index_stringl(subentries, (char *)to_add, to_add_len, 1);
				} else if (Z_TYPE_PP(data) == IS_STRING) {
					MAKE_STD_ZVAL(subentries);
					array_init(subentries);
					add_next_index_stringl(subentries, Z_STRVAL_PP(data), Z_STRLEN_PP(data), 1);
					add_next_index_stringl(subentries, (char *)to_add, to_add_len, 1);
					zend_hash_update(Z_ARRVAL_P(subitem), sname, strlen(sname)+1, &subentries, sizeof(zval*), NULL);
				}
			} else {
				add_assoc_stringl(subitem, sname, (char *)to_add, to_add_len, 1);
			}
		}
	}
	if (key != NULL) {
		zend_hash_update(HASH_OF(val), key, strlen(key) + 1, (void *)&subitem, sizeof(subitem), NULL);
	}
}
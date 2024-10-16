static int php_snmp_parse_oid(zval *object, int st, struct objid_query *objid_query, zval *oid, zval *type, zval *value)
{
	char *pptr;
	uint32_t idx_type = 0, idx_value = 0;
	zval *tmp_oid, *tmp_type, *tmp_value;

	if (Z_TYPE_P(oid) != IS_ARRAY) {
		convert_to_string_ex(oid);
	}

	if (st & SNMP_CMD_SET) {
		if (Z_TYPE_P(type) != IS_ARRAY) {
			convert_to_string_ex(type);
		}

		if (Z_TYPE_P(value) != IS_ARRAY) {
			convert_to_string_ex(value);
		}
	}

	objid_query->count = 0;
	objid_query->array_output = ((st & SNMP_CMD_WALK) ? TRUE : FALSE);
	if (Z_TYPE_P(oid) == IS_STRING) {
		objid_query->vars = (snmpobjarg *)emalloc(sizeof(snmpobjarg));
		if (objid_query->vars == NULL) {
			php_error_docref(NULL, E_WARNING, "emalloc() failed while parsing oid: %s", strerror(errno));
			efree(objid_query->vars);
			return FALSE;
		}
		objid_query->vars[objid_query->count].oid = Z_STRVAL_P(oid);
		if (st & SNMP_CMD_SET) {
			if (Z_TYPE_P(type) == IS_STRING && Z_TYPE_P(value) == IS_STRING) {
				if (Z_STRLEN_P(type) != 1) {
					php_error_docref(NULL, E_WARNING, "Bogus type '%s', should be single char, got %u", Z_STRVAL_P(type), Z_STRLEN_P(type));
					efree(objid_query->vars);
					return FALSE;
				}
				pptr = Z_STRVAL_P(type);
				objid_query->vars[objid_query->count].type = *pptr;
				objid_query->vars[objid_query->count].value = Z_STRVAL_P(value);
			} else {
				php_error_docref(NULL, E_WARNING, "Single objid and multiple type or values are not supported");
				efree(objid_query->vars);
				return FALSE;
			}
		}
		objid_query->count++;
	} else if (Z_TYPE_P(oid) == IS_ARRAY) { /* we got objid array */
		if (zend_hash_num_elements(Z_ARRVAL_P(oid)) == 0) {
			php_error_docref(NULL, E_WARNING, "Got empty OID array");
			return FALSE;
		}
		objid_query->vars = (snmpobjarg *)emalloc(sizeof(snmpobjarg) * zend_hash_num_elements(Z_ARRVAL_P(oid)));
		if (objid_query->vars == NULL) {
			php_error_docref(NULL, E_WARNING, "emalloc() failed while parsing oid array: %s", strerror(errno));
			efree(objid_query->vars);
			return FALSE;
		}
		objid_query->array_output = ( (st & SNMP_CMD_SET) ? FALSE : TRUE );
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(oid), tmp_oid) {
			convert_to_string_ex(tmp_oid);
			objid_query->vars[objid_query->count].oid = Z_STRVAL_P(tmp_oid);
			if (st & SNMP_CMD_SET) {
				if (Z_TYPE_P(type) == IS_STRING) {
					pptr = Z_STRVAL_P(type);
					objid_query->vars[objid_query->count].type = *pptr;
				} else if (Z_TYPE_P(type) == IS_ARRAY) {
					while (idx_type < Z_ARRVAL_P(type)->nNumUsed) {
						tmp_type = &Z_ARRVAL_P(type)->arData[idx_type].val;
						if (Z_TYPE_P(tmp_type) != IS_UNDEF) {
							break;
						}
						idx_type++;
					}
					if (idx_type < Z_ARRVAL_P(type)->nNumUsed) {
						convert_to_string_ex(tmp_type);
						if (Z_STRLEN_P(tmp_type) != 1) {
							php_error_docref(NULL, E_WARNING, "'%s': bogus type '%s', should be single char, got %u", Z_STRVAL_P(tmp_oid), Z_STRVAL_P(tmp_type), Z_STRLEN_P(tmp_type));
							efree(objid_query->vars);
							return FALSE;
						}
						pptr = Z_STRVAL_P(tmp_type);
						objid_query->vars[objid_query->count].type = *pptr;
						idx_type++;
					} else {
						php_error_docref(NULL, E_WARNING, "'%s': no type set", Z_STRVAL_P(tmp_oid));
						efree(objid_query->vars);
						return FALSE;
					}
				}

				if (Z_TYPE_P(value) == IS_STRING) {
					objid_query->vars[objid_query->count].value = Z_STRVAL_P(value);
				} else if (Z_TYPE_P(value) == IS_ARRAY) {
					while (idx_value < Z_ARRVAL_P(value)->nNumUsed) {
						tmp_value = &Z_ARRVAL_P(value)->arData[idx_value].val;
						if (Z_TYPE_P(tmp_value) != IS_UNDEF) {
							break;
						}
						idx_value++;
					}
					if (idx_value < Z_ARRVAL_P(value)->nNumUsed) {
						convert_to_string_ex(tmp_value);
						objid_query->vars[objid_query->count].value = Z_STRVAL_P(tmp_value);
						idx_value++;
					} else {
						php_error_docref(NULL, E_WARNING, "'%s': no value set", Z_STRVAL_P(tmp_oid));
						efree(objid_query->vars);
						return FALSE;
					}
				}
			}
			objid_query->count++;
		} ZEND_HASH_FOREACH_END();
	}

	/* now parse all OIDs */
	if (st & SNMP_CMD_WALK) {
		if (objid_query->count > 1) {
			php_snmp_error(object, NULL, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Multi OID walks are not supported!");
			efree(objid_query->vars);
			return FALSE;
		}
		objid_query->vars[0].name_length = MAX_NAME_LEN;
		if (strlen(objid_query->vars[0].oid)) { /* on a walk, an empty string means top of tree - no error */
			if (!snmp_parse_oid(objid_query->vars[0].oid, objid_query->vars[0].name, &(objid_query->vars[0].name_length))) {
				php_snmp_error(object, NULL, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Invalid object identifier: %s", objid_query->vars[0].oid);
				efree(objid_query->vars);
				return FALSE;
			}
		} else {
			memmove((char *)objid_query->vars[0].name, (char *)objid_mib, sizeof(objid_mib));
			objid_query->vars[0].name_length = sizeof(objid_mib) / sizeof(oid);
		}
	} else {
		for (objid_query->offset = 0; objid_query->offset < objid_query->count; objid_query->offset++) {
			objid_query->vars[objid_query->offset].name_length = MAX_OID_LEN;
			if (!snmp_parse_oid(objid_query->vars[objid_query->offset].oid, objid_query->vars[objid_query->offset].name, &(objid_query->vars[objid_query->offset].name_length))) {
				php_snmp_error(object, NULL, PHP_SNMP_ERRNO_OID_PARSING_ERROR, "Invalid object identifier: %s", objid_query->vars[objid_query->offset].oid);
				efree(objid_query->vars);
				return FALSE;
			}
		}
	}
	objid_query->offset = 0;
	objid_query->step = objid_query->count;
	return (objid_query->count > 0);
}
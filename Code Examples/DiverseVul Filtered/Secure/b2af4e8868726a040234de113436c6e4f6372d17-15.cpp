PHPAPI void php_debug_zval_dump(zval *struc, int level) /* {{{ */
{
	HashTable *myht = NULL;
	zend_string *class_name;
	int is_temp = 0;
	int is_ref = 0;
	zend_ulong index;
	zend_string *key;
	zval *val;
	uint32_t count;

	if (level > 1) {
		php_printf("%*c", level - 1, ' ');
	}

again:
	switch (Z_TYPE_P(struc)) {
	case IS_FALSE:
		php_printf("%sbool(false)\n", COMMON);
		break;
	case IS_TRUE:
		php_printf("%sbool(true)\n", COMMON);
		break;
	case IS_NULL:
		php_printf("%sNULL\n", COMMON);
		break;
	case IS_LONG:
		php_printf("%sint(" ZEND_LONG_FMT ")\n", COMMON, Z_LVAL_P(struc));
		break;
	case IS_DOUBLE:
		php_printf("%sfloat(%.*G)\n", COMMON, (int) EG(precision), Z_DVAL_P(struc));
		break;
	case IS_STRING:
		php_printf("%sstring(%zd) \"", COMMON, Z_STRLEN_P(struc));
		PHPWRITE(Z_STRVAL_P(struc), Z_STRLEN_P(struc));
		php_printf("\" refcount(%u)\n", Z_REFCOUNTED_P(struc) ? Z_REFCOUNT_P(struc) : 1);
		break;
	case IS_ARRAY:
		myht = Z_ARRVAL_P(struc);
		if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht) && myht->u.v.nApplyCount++ > 1) {
			myht->u.v.nApplyCount--;
			PUTS("*RECURSION*\n");
			return;
		}
		count = zend_array_count(myht);
		php_printf("%sarray(%d) refcount(%u){\n", COMMON, count, Z_REFCOUNTED_P(struc) ? Z_REFCOUNT_P(struc) : 1);
		ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
			zval_array_element_dump(val, index, key, level);
		} ZEND_HASH_FOREACH_END();
		if (level > 1 && ZEND_HASH_APPLY_PROTECTION(myht)) {
			myht->u.v.nApplyCount--;
		}
		if (is_temp) {
			zend_hash_destroy(myht);
			efree(myht);
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_OBJECT:
		myht = Z_OBJDEBUG_P(struc, is_temp);
		if (myht) {
			if (myht->u.v.nApplyCount > 1) {
				PUTS("*RECURSION*\n");
				return;
			} else {
				myht->u.v.nApplyCount++;
			}
		}
		class_name = Z_OBJ_HANDLER_P(struc, get_class_name)(Z_OBJ_P(struc));
		php_printf("%sobject(%s)#%d (%d) refcount(%u){\n", COMMON, ZSTR_VAL(class_name), Z_OBJ_HANDLE_P(struc), myht ? zend_array_count(myht) : 0, Z_REFCOUNT_P(struc));
		zend_string_release(class_name);
		if (myht) {
			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
				zval_object_property_dump(val, index, key, level);
			} ZEND_HASH_FOREACH_END();
			myht->u.v.nApplyCount--;
			if (is_temp) {
				zend_hash_destroy(myht);
				efree(myht);
			}
		}
		if (level > 1) {
			php_printf("%*c", level - 1, ' ');
		}
		PUTS("}\n");
		break;
	case IS_RESOURCE: {
		const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(struc));
		php_printf("%sresource(%d) of type (%s) refcount(%u)\n", COMMON, Z_RES_P(struc)->handle, type_name ? type_name : "Unknown", Z_REFCOUNT_P(struc));
		break;
	}
	case IS_REFERENCE:
		//??? hide references with refcount==1 (for compatibility)
		if (Z_REFCOUNT_P(struc) > 1) {
			is_ref = 1;
		}
		struc = Z_REFVAL_P(struc);
		goto again;
	default:
		php_printf("%sUNKNOWN:0\n", COMMON);
		break;
	}
}
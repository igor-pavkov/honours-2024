zend_function *spl_filesystem_object_get_method_check(zend_object **object, zend_string *method, const zval *key) /* {{{ */
{
	spl_filesystem_object *fsobj = spl_filesystem_from_obj(*object);

	if (fsobj->u.dir.dirp == NULL && fsobj->orig_path == NULL) {
		zend_function *func;
		zend_string *tmp = zend_string_init("_bad_state_ex", sizeof("_bad_state_ex") - 1, 0);
		func = zend_get_std_object_handlers()->get_method(object, tmp, NULL);
		zend_string_release(tmp);
		return func;
	}

	return zend_get_std_object_handlers()->get_method(object, method, key);
}
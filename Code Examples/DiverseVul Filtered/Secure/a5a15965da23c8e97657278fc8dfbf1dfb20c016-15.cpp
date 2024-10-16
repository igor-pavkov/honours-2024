static void spl_filesystem_tree_it_current_key(zend_object_iterator *iter, zval *key)
{
	spl_filesystem_object *object = spl_filesystem_iterator_to_object((spl_filesystem_iterator *)iter);

	if (SPL_FILE_DIR_KEY(object, SPL_FILE_DIR_KEY_AS_FILENAME)) {
		ZVAL_STRING(key, object->u.dir.entry.d_name);
	} else {
		spl_filesystem_object_get_file_name(object);
		ZVAL_STRINGL(key, object->file_name, object->file_name_len);
	}
}
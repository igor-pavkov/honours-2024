static HashTable *spl_array_get_gc(zval *object, zval ***gc_data, int *gc_data_count TSRMLS_DC) /* {{{ */
{
	spl_array_object *intern = (spl_array_object*)zend_object_store_get_object(object TSRMLS_CC);

	*gc_data = &intern->array;
	*gc_data_count = 1;
	return zend_std_get_properties(object);
}
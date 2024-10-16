PHP_METHOD(PharFileInfo, __destruct)
{
	phar_entry_object *entry_obj = (phar_entry_object*)zend_object_store_get_object(getThis() TSRMLS_CC); \

	if (entry_obj->ent.entry && entry_obj->ent.entry->is_temp_dir) {
		if (entry_obj->ent.entry->filename) {
			efree(entry_obj->ent.entry->filename);
			entry_obj->ent.entry->filename = NULL;
		}

		efree(entry_obj->ent.entry);
		entry_obj->ent.entry = NULL;
	}
}
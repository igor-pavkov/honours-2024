static zend_object *spl_filesystem_object_new_ex(zend_class_entry *class_type)
{
	spl_filesystem_object *intern;

	intern = ecalloc(1, sizeof(spl_filesystem_object) + zend_object_properties_size(class_type));
	/* intern->type = SPL_FS_INFO; done by set 0 */
	intern->file_class = spl_ce_SplFileObject;
	intern->info_class = spl_ce_SplFileInfo;

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->std.handlers = &spl_filesystem_object_handlers;

	return &intern->std;
}
static zend_object *spl_filesystem_object_new_check(zend_class_entry *class_type)
{
	spl_filesystem_object *ret = spl_filesystem_from_obj(spl_filesystem_object_new_ex(class_type));
	ret->std.handlers = &spl_filesystem_object_check_handlers;
	return &ret->std;
}
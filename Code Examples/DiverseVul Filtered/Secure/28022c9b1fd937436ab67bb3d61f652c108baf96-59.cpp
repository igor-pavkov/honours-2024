static void php_free_gd_image(zend_resource *rsrc)
{
	gdImageDestroy((gdImagePtr) rsrc->ptr);
}
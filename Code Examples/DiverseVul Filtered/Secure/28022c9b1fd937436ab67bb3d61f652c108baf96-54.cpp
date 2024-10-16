static void php_free_gd_font(zend_resource *rsrc)
{
	gdFontPtr fp = (gdFontPtr) rsrc->ptr;

	if (fp->data) {
		efree(fp->data);
	}

	efree(fp);
}
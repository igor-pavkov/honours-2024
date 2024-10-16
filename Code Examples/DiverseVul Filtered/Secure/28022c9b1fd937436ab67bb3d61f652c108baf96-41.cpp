PHP_FUNCTION(imagetypes)
{
	int ret=0;
	ret = 1;
#ifdef HAVE_GD_JPG
	ret |= 2;
#endif
#ifdef HAVE_GD_PNG
	ret |= 4;
#endif
	ret |= 8;
#if defined(HAVE_GD_XPM)
	ret |= 16;
#endif
#ifdef HAVE_GD_WEBP
	ret |= 32;
#endif

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(ret);
}
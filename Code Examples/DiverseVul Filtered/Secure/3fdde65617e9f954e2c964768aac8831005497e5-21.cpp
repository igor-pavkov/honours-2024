static PHP_GINIT_FUNCTION(exif)
{
#if defined(COMPILE_DL_EXIF) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	exif_globals->encode_unicode    = NULL;
	exif_globals->decode_unicode_be = NULL;
	exif_globals->decode_unicode_le = NULL;
	exif_globals->encode_jis        = NULL;
	exif_globals->decode_jis_be     = NULL;
	exif_globals->decode_jis_le     = NULL;
}
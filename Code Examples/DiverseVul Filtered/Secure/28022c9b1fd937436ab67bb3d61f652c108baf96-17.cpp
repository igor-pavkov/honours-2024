static void _php_image_create_from(INTERNAL_FUNCTION_PARAMETERS, int image_type, char *tn, gdImagePtr (*func_p)(), gdImagePtr (*ioctx_func_p)())
{
	char *file;
	size_t file_len;
	zend_long srcx, srcy, width, height;
	gdImagePtr im = NULL;
	php_stream *stream;
	FILE * fp = NULL;
#ifdef HAVE_GD_JPG
	long ignore_warning;
#endif

	if (image_type == PHP_GDIMG_TYPE_GD2PART) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "pllll", &file, &file_len, &srcx, &srcy, &width, &height) == FAILURE) {
			return;
		}
		if (width < 1 || height < 1) {
			php_error_docref(NULL, E_WARNING, "Zero width or height not allowed");
			RETURN_FALSE;
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &file, &file_len) == FAILURE) {
			return;
		}
	}


	stream = php_stream_open_wrapper(file, "rb", REPORT_ERRORS|IGNORE_PATH|IGNORE_URL_WIN, NULL);
	if (stream == NULL)	{
		RETURN_FALSE;
	}

	/* try and avoid allocating a FILE* if the stream is not naturally a FILE* */
	if (php_stream_is(stream, PHP_STREAM_IS_STDIO))	{
		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO, (void**)&fp, REPORT_ERRORS)) {
			goto out_err;
		}
	} else if (ioctx_func_p) {
		/* we can create an io context */
		gdIOCtx* io_ctx;
		zend_string *buff;
		char *pstr;

		buff = php_stream_copy_to_mem(stream, PHP_STREAM_COPY_ALL, 0);

		if (!buff) {
			php_error_docref(NULL, E_WARNING,"Cannot read image data");
			goto out_err;
		}

		/* needs to be malloc (persistent) - GD will free() it later */
		pstr = pestrndup(ZSTR_VAL(buff), ZSTR_LEN(buff), 1);
		io_ctx = gdNewDynamicCtxEx(ZSTR_LEN(buff), pstr, 0);
		if (!io_ctx) {
			pefree(pstr, 1);
			zend_string_release(buff);
			php_error_docref(NULL, E_WARNING,"Cannot allocate GD IO context");
			goto out_err;
		}

		if (image_type == PHP_GDIMG_TYPE_GD2PART) {
			im = (*ioctx_func_p)(io_ctx, srcx, srcy, width, height);
		} else {
			im = (*ioctx_func_p)(io_ctx);
		}
		io_ctx->gd_free(io_ctx);
		pefree(pstr, 1);
		zend_string_release(buff);
	}
	else if (php_stream_can_cast(stream, PHP_STREAM_AS_STDIO)) {
		/* try and force the stream to be FILE* */
		if (FAILURE == php_stream_cast(stream, PHP_STREAM_AS_STDIO | PHP_STREAM_CAST_TRY_HARD, (void **) &fp, REPORT_ERRORS)) {
			goto out_err;
		}
	}

	if (!im && fp) {
		switch (image_type) {
			case PHP_GDIMG_TYPE_GD2PART:
				im = (*func_p)(fp, srcx, srcy, width, height);
				break;
#if defined(HAVE_GD_XPM)
			case PHP_GDIMG_TYPE_XPM:
				im = gdImageCreateFromXpm(file);
				break;
#endif

#ifdef HAVE_GD_JPG
			case PHP_GDIMG_TYPE_JPG:
				ignore_warning = INI_INT("gd.jpeg_ignore_warning");
				im = gdImageCreateFromJpegEx(fp, ignore_warning);
			break;
#endif

			default:
				im = (*func_p)(fp);
				break;
		}

		fflush(fp);
	}

/* register_im: */
	if (im) {
		RETVAL_RES(zend_register_resource(im, le_gd));
		php_stream_close(stream);
		return;
	}

	php_error_docref(NULL, E_WARNING, "'%s' is not a valid %s file", file, tn);
out_err:
	php_stream_close(stream);
	RETURN_FALSE;

}
static void phar_add_file(phar_archive_data **pphar, char *filename, int filename_len, char *cont_str, size_t cont_len, zval *zresource)
{
	int start_pos=0;
	char *error;
	size_t contents_len;
	phar_entry_data *data;
	php_stream *contents_file = NULL;
	php_stream_statbuf ssb;

	if (filename_len >= (int)sizeof(".phar")-1) {
		start_pos = ('/' == filename[0] ? 1 : 0); /* account for any leading slash: multiple-leads handled elsewhere */
		if (!memcmp(&filename[start_pos], ".phar", sizeof(".phar")-1) && (filename[start_pos+5] == '/' || filename[start_pos+5] == '\\' || filename[start_pos+5] == '\0')) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Cannot create any files in magic \".phar\" directory");
			return;
		}
	}

	if (!(data = phar_get_or_create_entry_data((*pphar)->fname, (*pphar)->fname_len, filename, filename_len, "w+b", 0, &error, 1))) {
		if (error) {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Entry %s does not exist and cannot be created: %s", filename, error);
			efree(error);
		} else {
			zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Entry %s does not exist and cannot be created", filename);
		}
		return;
	} else {
		if (error) {
			efree(error);
		}

		if (!data->internal_file->is_dir) {
			if (cont_str) {
				contents_len = php_stream_write(data->fp, cont_str, cont_len);
				if (contents_len != cont_len) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Entry %s could not be written to", filename);
					return;
				}
			} else {
				if (!(php_stream_from_zval_no_verify(contents_file, zresource))) {
					zend_throw_exception_ex(spl_ce_BadMethodCallException, 0, "Entry %s could not be written to", filename);
					return;
				}
				php_stream_copy_to_stream_ex(contents_file, data->fp, PHP_STREAM_COPY_ALL, &contents_len);
			}
			data->internal_file->compressed_filesize = data->internal_file->uncompressed_filesize = contents_len;
		}

		if (contents_file != NULL && php_stream_stat(contents_file, &ssb) != -1) {
			data->internal_file->flags = ssb.sb.st_mode & PHAR_ENT_PERM_MASK ;
		} else {
#ifndef _WIN32
			mode_t mask;
			mask = umask(0);
			umask(mask);
			data->internal_file->flags &= ~mask;
#endif
		}

		/* check for copy-on-write */
		if (pphar[0] != data->phar) {
			*pphar = data->phar;
		}
		phar_entry_delref(data);
		phar_flush(*pphar, 0, 0, 0, &error);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0, "%s", error);
			efree(error);
		}
	}
}
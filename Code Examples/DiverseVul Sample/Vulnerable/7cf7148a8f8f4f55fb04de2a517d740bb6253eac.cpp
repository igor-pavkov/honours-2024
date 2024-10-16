static php_iconv_err_t _php_iconv_mime_decode(smart_str *pretval, const char *str, size_t str_nbytes, const char *enc, const char **next_pos, int mode)
{
	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	iconv_t cd = (iconv_t)(-1), cd_pl = (iconv_t)(-1);

	const char *p1;
	size_t str_left;
	unsigned int scan_stat = 0;
	const char *csname = NULL;
	size_t csname_len;
	const char *encoded_text = NULL;
	size_t encoded_text_len = 0;
	const char *encoded_word = NULL;
	const char *spaces = NULL;

	php_iconv_enc_scheme_t enc_scheme = PHP_ICONV_ENC_SCHEME_BASE64;

	if (next_pos != NULL) {
		*next_pos = NULL;
	}

	cd_pl = iconv_open(enc, ICONV_ASCII_ENCODING);

	if (cd_pl == (iconv_t)(-1)) {
#if ICONV_SUPPORTS_ERRNO
		if (errno == EINVAL) {
			err = PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			err = PHP_ICONV_ERR_CONVERTER;
		}
#else
		err = PHP_ICONV_ERR_UNKNOWN;
#endif
		goto out;
	}

	p1 = str;
	for (str_left = str_nbytes; str_left > 0; str_left--, p1++) {
		int eos = 0;

		switch (scan_stat) {
			case 0: /* expecting any character */
				switch (*p1) {
					case '\r': /* part of an EOL sequence? */
						scan_stat = 7;
						break;

					case '\n':
						scan_stat = 8;
						break;

					case '=': /* first letter of an encoded chunk */
						encoded_word = p1;
						scan_stat = 1;
						break;

					case ' ': case '\t': /* a chunk of whitespaces */
						spaces = p1;
						scan_stat = 11;
						break;

					default: /* first letter of a non-encoded word */
						err = _php_iconv_appendc(pretval, *p1, cd_pl);
						if (err != PHP_ICONV_ERR_SUCCESS) {
							if (mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR) {
								err = PHP_ICONV_ERR_SUCCESS;
							} else {
								goto out;
							}
						}
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						}
						break;
				}
				break;

			case 1: /* expecting a delimiter */
				if (*p1 != '?') {
					if (*p1 == '\r' || *p1 == '\n') {
						--p1;
					}
					err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
					if (err != PHP_ICONV_ERR_SUCCESS) {
						goto out;
					}
					encoded_word = NULL;
					if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
						scan_stat = 12;
					} else {
						scan_stat = 0;
					}
					break;
				}
				csname = p1 + 1;
				scan_stat = 2;
				break;

			case 2: /* expecting a charset name */
				switch (*p1) {
					case '?': /* normal delimiter: encoding scheme follows */
						scan_stat = 3;
						break;

					case '*': /* new style delimiter: locale id follows */
						scan_stat = 10;
						break;

					case '\r': case '\n': /* not an encoded-word */
						--p1;
						_php_iconv_appendc(pretval, '=', cd_pl);
						_php_iconv_appendc(pretval, '?', cd_pl);
						err = _php_iconv_appendl(pretval, csname, (size_t)((p1 + 1) - csname), cd_pl);
						if (err != PHP_ICONV_ERR_SUCCESS) {
							goto out;
						}
						csname = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						}
						else {
							scan_stat = 0;
						}
						continue;
				}
				if (scan_stat != 2) {
					char tmpbuf[80];

					if (csname == NULL) {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}

					csname_len = (size_t)(p1 - csname);

					if (csname_len > sizeof(tmpbuf) - 1) {
						if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
							err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
							if (err != PHP_ICONV_ERR_SUCCESS) {
								goto out;
							}
							encoded_word = NULL;
							if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
								scan_stat = 12;
							} else {
								scan_stat = 0;
							}
							break;
						} else {
							err = PHP_ICONV_ERR_MALFORMED;
							goto out;
						}
					}

					memcpy(tmpbuf, csname, csname_len);
					tmpbuf[csname_len] = '\0';

					if (cd != (iconv_t)(-1)) {
						iconv_close(cd);
					}

					cd = iconv_open(enc, tmpbuf);

					if (cd == (iconv_t)(-1)) {
						if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
							/* Bad character set, but the user wants us to
							 * press on. In this case, we'll just insert the
							 * undecoded encoded word, since there isn't really
							 * a more sensible behaviour available; the only
							 * other options are to swallow the encoded word
							 * entirely or decode it with an arbitrarily chosen
							 * single byte encoding, both of which seem to have
							 * a higher WTF factor than leaving it undecoded.
							 *
							 * Given this approach, we need to skip ahead to
							 * the end of the encoded word. */
							int qmarks = 2;
							while (qmarks > 0 && str_left > 1) {
								if (*(++p1) == '?') {
									--qmarks;
								}
								--str_left;
							}

							/* Look ahead to check for the terminating = that
							 * should be there as well; if it's there, we'll
							 * also include that. If it's not, there isn't much
							 * we can do at this point. */
							if (*(p1 + 1) == '=') {
								++p1;
								--str_left;
							}

							err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
							if (err != PHP_ICONV_ERR_SUCCESS) {
								goto out;
							}

							/* Let's go back and see if there are further
							 * encoded words or bare content, and hope they
							 * might actually have a valid character set. */
							scan_stat = 12;
							break;
						} else {
#if ICONV_SUPPORTS_ERRNO
							if (errno == EINVAL) {
								err = PHP_ICONV_ERR_WRONG_CHARSET;
							} else {
								err = PHP_ICONV_ERR_CONVERTER;
							}
#else
							err = PHP_ICONV_ERR_UNKNOWN;
#endif
							goto out;
						}
					}
				}
				break;

			case 3: /* expecting a encoding scheme specifier */
				switch (*p1) {
					case 'b':
					case 'B':
						enc_scheme = PHP_ICONV_ENC_SCHEME_BASE64;
						scan_stat = 4;
						break;

					case 'q':
					case 'Q':
						enc_scheme = PHP_ICONV_ENC_SCHEME_QPRINT;
						scan_stat = 4;
						break;

					default:
						if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
							err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
							if (err != PHP_ICONV_ERR_SUCCESS) {
								goto out;
							}
							encoded_word = NULL;
							if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
								scan_stat = 12;
							} else {
								scan_stat = 0;
							}
							break;
						} else {
							err = PHP_ICONV_ERR_MALFORMED;
							goto out;
						}
				}
				break;

			case 4: /* expecting a delimiter */
				if (*p1 != '?') {
					if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
						/* pass the entire chunk through the converter */
						err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
						if (err != PHP_ICONV_ERR_SUCCESS) {
							goto out;
						}
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						} else {
							scan_stat = 0;
						}
						break;
					} else {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}
				}
				encoded_text = p1 + 1;
				scan_stat = 5;
				break;

			case 5: /* expecting an encoded portion */
				if (*p1 == '?') {
					encoded_text_len = (size_t)(p1 - encoded_text);
					scan_stat = 6;
				}
				break;

			case 7: /* expecting a "\n" character */
				if (*p1 == '\n') {
					scan_stat = 8;
				} else {
					/* bare CR */
					_php_iconv_appendc(pretval, '\r', cd_pl);
					_php_iconv_appendc(pretval, *p1, cd_pl);
					scan_stat = 0;
				}
				break;

			case 8: /* checking whether the following line is part of a
					   folded header */
				if (*p1 != ' ' && *p1 != '\t') {
					--p1;
					str_left = 1; /* quit_loop */
					break;
				}
				if (encoded_word == NULL) {
					_php_iconv_appendc(pretval, ' ', cd_pl);
				}
				spaces = NULL;
				scan_stat = 11;
				break;

			case 6: /* expecting a End-Of-Chunk character "=" */
				if (*p1 != '=') {
					if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
						/* pass the entire chunk through the converter */
						err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
						if (err != PHP_ICONV_ERR_SUCCESS) {
							goto out;
						}
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						} else {
							scan_stat = 0;
						}
						break;
					} else {
						err = PHP_ICONV_ERR_MALFORMED;
						goto out;
					}
				}
				scan_stat = 9;
				if (str_left == 1) {
					eos = 1;
				} else {
					break;
				}

			case 9: /* choice point, seeing what to do next.*/
				switch (*p1) {
					default:
						/* Handle non-RFC-compliant formats
						 *
						 * RFC2047 requires the character that comes right
						 * after an encoded word (chunk) to be a whitespace,
						 * while there are lots of broken implementations that
						 * generate such malformed headers that don't fulfill
						 * that requirement.
						 */
						if (!eos) {
							if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
								/* pass the entire chunk through the converter */
								err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
								if (err != PHP_ICONV_ERR_SUCCESS) {
									goto out;
								}
								scan_stat = 12;
								break;
							}
						}
						/* break is omitted intentionally */

					case '\r': case '\n': case ' ': case '\t': {
						zend_string *decoded_text;

						switch (enc_scheme) {
							case PHP_ICONV_ENC_SCHEME_BASE64:
								decoded_text = php_base64_decode((unsigned char*)encoded_text, encoded_text_len);
								break;

							case PHP_ICONV_ENC_SCHEME_QPRINT:
								decoded_text = php_quot_print_decode((unsigned char*)encoded_text, encoded_text_len, 1);
								break;
							default:
								decoded_text = NULL;
								break;
						}

						if (decoded_text == NULL) {
							if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
								/* pass the entire chunk through the converter */
								err = _php_iconv_appendl(pretval, encoded_word, (size_t)((p1 + 1) - encoded_word), cd_pl);
								if (err != PHP_ICONV_ERR_SUCCESS) {
									goto out;
								}
								encoded_word = NULL;
								if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
									scan_stat = 12;
								} else {
									scan_stat = 0;
								}
								break;
							} else {
								err = PHP_ICONV_ERR_UNKNOWN;
								goto out;
							}
						}

						err = _php_iconv_appendl(pretval, ZSTR_VAL(decoded_text), ZSTR_LEN(decoded_text), cd);
						zend_string_release(decoded_text);

						if (err != PHP_ICONV_ERR_SUCCESS) {
							if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
								/* pass the entire chunk through the converter */
								err = _php_iconv_appendl(pretval, encoded_word, (size_t)(p1 - encoded_word), cd_pl);
								encoded_word = NULL;
								if (err != PHP_ICONV_ERR_SUCCESS) {
									break;
								}
							} else {
								goto out;
							}
						}

						if (eos) { /* reached end-of-string. done. */
							scan_stat = 0;
							break;
						}

						switch (*p1) {
							case '\r': /* part of an EOL sequence? */
								scan_stat = 7;
								break;

							case '\n':
								scan_stat = 8;
								break;

							case '=': /* first letter of an encoded chunk */
								scan_stat = 1;
								break;

							case ' ': case '\t': /* medial whitespaces */
								spaces = p1;
								scan_stat = 11;
								break;

							default: /* first letter of a non-encoded word */
								_php_iconv_appendc(pretval, *p1, cd_pl);
								scan_stat = 12;
								break;
						}
					} break;
				}
				break;

			case 10: /* expects a language specifier. dismiss it for now */
				if (*p1 == '?') {
					scan_stat = 3;
				}
				break;

			case 11: /* expecting a chunk of whitespaces */
				switch (*p1) {
					case '\r': /* part of an EOL sequence? */
						scan_stat = 7;
						break;

					case '\n':
						scan_stat = 8;
						break;

					case '=': /* first letter of an encoded chunk */
						if (spaces != NULL && encoded_word == NULL) {
							_php_iconv_appendl(pretval, spaces, (size_t)(p1 - spaces), cd_pl);
							spaces = NULL;
						}
						encoded_word = p1;
						scan_stat = 1;
						break;

					case ' ': case '\t':
						break;

					default: /* first letter of a non-encoded word */
						if (spaces != NULL) {
							_php_iconv_appendl(pretval, spaces, (size_t)(p1 - spaces), cd_pl);
							spaces = NULL;
						}
						_php_iconv_appendc(pretval, *p1, cd_pl);
						encoded_word = NULL;
						if ((mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							scan_stat = 12;
						} else {
							scan_stat = 0;
						}
						break;
				}
				break;

			case 12: /* expecting a non-encoded word */
				switch (*p1) {
					case '\r': /* part of an EOL sequence? */
						scan_stat = 7;
						break;

					case '\n':
						scan_stat = 8;
						break;

					case ' ': case '\t':
						spaces = p1;
						scan_stat = 11;
						break;

					case '=': /* first letter of an encoded chunk */
						if (!(mode & PHP_ICONV_MIME_DECODE_STRICT)) {
							encoded_word = p1;
							scan_stat = 1;
							break;
						}
						/* break is omitted intentionally */

					default:
						_php_iconv_appendc(pretval, *p1, cd_pl);
						break;
				}
				break;
		}
	}
	switch (scan_stat) {
		case 0: case 8: case 11: case 12:
			break;
		default:
			if ((mode & PHP_ICONV_MIME_DECODE_CONTINUE_ON_ERROR)) {
				if (scan_stat == 1) {
					_php_iconv_appendc(pretval, '=', cd_pl);
				}
				err = PHP_ICONV_ERR_SUCCESS;
			} else {
				err = PHP_ICONV_ERR_MALFORMED;
				goto out;
			}
	}

	if (next_pos != NULL) {
		*next_pos = p1;
	}

	smart_str_0(pretval);
out:
	if (cd != (iconv_t)(-1)) {
		iconv_close(cd);
	}
	if (cd_pl != (iconv_t)(-1)) {
		iconv_close(cd_pl);
	}
	return err;
}
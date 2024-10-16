static int process_request(
	/*! [in] HTTP Request message. */
	http_message_t *req,
	/*! [out] Tpye of response. */
	enum resp_type *rtype,
	/*! [out] Headers. */
	membuffer *headers,
	/*! [out] Get filename from request document. */
	membuffer *filename,
	/*! [out] Xml alias document from the request document. */
	struct xml_alias_t *alias,
	/*! [out] Send Instruction object where the response is set up. */
	struct SendInstruction *RespInstr)
{
	int code;
	int err_code;

	char *request_doc;
	UpnpFileInfo *finfo;
	time_t aux_LastModified;
	int using_alias;
	int using_virtual_dir;
	uri_type *url;
	const char *temp_str;
	int resp_major;
	int resp_minor;
	int alias_grabbed;
	size_t dummy;
	const char *extra_headers = NULL;

	print_http_headers(req);
	url = &req->uri;
	assert(req->method == HTTPMETHOD_GET ||
	       req->method == HTTPMETHOD_HEAD ||
	       req->method == HTTPMETHOD_POST ||
	       req->method == HTTPMETHOD_SIMPLEGET);
	/* init */
	memset(&finfo, 0, sizeof(finfo));
	request_doc = NULL;
	finfo = UpnpFileInfo_new();
	alias_grabbed = FALSE;
	err_code = HTTP_INTERNAL_SERVER_ERROR;	/* default error */
	using_virtual_dir = FALSE;
	using_alias = FALSE;

	http_CalcResponseVersion(req->major_version, req->minor_version,
				 &resp_major, &resp_minor);
	/* */
	/* remove dots */
	/* */
	request_doc = malloc(url->pathquery.size + 1);
	if (request_doc == NULL) {
		goto error_handler;	/* out of mem */
	}
	memcpy(request_doc, url->pathquery.buff, url->pathquery.size);
	request_doc[url->pathquery.size] = '\0';
	dummy = url->pathquery.size;
	remove_escaped_chars(request_doc, &dummy);
	code = remove_dots(request_doc, url->pathquery.size);
	if (code != 0) {
		err_code = HTTP_FORBIDDEN;
		goto error_handler;
	}
	if (*request_doc != '/') {
		/* no slash */
		err_code = HTTP_BAD_REQUEST;
		goto error_handler;
	}
	if (isFileInVirtualDir(request_doc)) {
		using_virtual_dir = TRUE;
		RespInstr->IsVirtualFile = 1;
		if (membuffer_assign_str(filename, request_doc) != 0) {
			goto error_handler;
		}
	} else {
		/* try using alias */
		if (is_valid_alias(&gAliasDoc)) {
			alias_grab(alias);
			alias_grabbed = TRUE;
			using_alias = get_alias(request_doc, alias, finfo);
			if (using_alias == TRUE) {
				UpnpFileInfo_set_ContentType(finfo,
					"text/xml; charset=\"utf-8\"");
				if (UpnpFileInfo_get_ContentType(finfo) == NULL) {
					goto error_handler;
				}
			}
		}
	}
	if (using_virtual_dir) {
		if (req->method != HTTPMETHOD_POST) {
			/* get file info */
			if (virtualDirCallback.
			    get_info(filename->buf, finfo) != 0) {
				err_code = HTTP_NOT_FOUND;
				goto error_handler;
			}
			/* try index.html if req is a dir */
			if (UpnpFileInfo_get_IsDirectory(finfo)) {
				if (filename->buf[filename->length - 1] == '/') {
					temp_str = "index.html";
				} else {
					temp_str = "/index.html";
				}
				if (membuffer_append_str(filename, temp_str) !=
				    0) {
					goto error_handler;
				}
				/* get info */
				if (virtualDirCallback.get_info(filename->buf, finfo) != UPNP_E_SUCCESS ||
				    UpnpFileInfo_get_IsDirectory(finfo)) {
					err_code = HTTP_NOT_FOUND;
					goto error_handler;
				}
			}
			/* not readable */
			if (!UpnpFileInfo_get_IsReadable(finfo)) {
				err_code = HTTP_FORBIDDEN;
				goto error_handler;
			}
			/* finally, get content type */
			/* if ( get_content_type(filename->buf, &content_type) != 0 ) */
			/*{ */
			/*  goto error_handler; */
			/* } */
		}
	} else if (!using_alias) {
		if (gDocumentRootDir.length == 0) {
			goto error_handler;
		}
		/* */
		/* get file name */
		/* */

		/* filename str */
		if (membuffer_assign_str(filename, gDocumentRootDir.buf) != 0 ||
		    membuffer_append_str(filename, request_doc) != 0) {
			goto error_handler;	/* out of mem */
		}
		/* remove trailing slashes */
		while (filename->length > 0 &&
		       filename->buf[filename->length - 1] == '/') {
			membuffer_delete(filename, filename->length - 1, 1);
		}
		if (req->method != HTTPMETHOD_POST) {
			/* get info on file */
			if (get_file_info(filename->buf, finfo) != 0) {
				err_code = HTTP_NOT_FOUND;
				goto error_handler;
			}
			/* try index.html if req is a dir */
			if (UpnpFileInfo_get_IsDirectory(finfo)) {
				if (filename->buf[filename->length - 1] == '/') {
					temp_str = "index.html";
				} else {
					temp_str = "/index.html";
				}
				if (membuffer_append_str(filename, temp_str) !=
				    0) {
					goto error_handler;
				}
				/* get info */
				if (get_file_info(filename->buf, finfo) != 0 ||
				    UpnpFileInfo_get_IsDirectory(finfo)) {
					err_code = HTTP_NOT_FOUND;
					goto error_handler;
				}
			}
			/* not readable */
			if (!UpnpFileInfo_get_IsReadable(finfo)) {
				err_code = HTTP_FORBIDDEN;
				goto error_handler;
			}
		}
		/* finally, get content type */
		/*      if ( get_content_type(filename->buf, &content_type) != 0 ) */
		/*      { */
		/*          goto error_handler; */
		/*      } */
	}
	RespInstr->ReadSendSize = UpnpFileInfo_get_FileLength(finfo);
	/* Check other header field. */
	code = CheckOtherHTTPHeaders(req, RespInstr,
		UpnpFileInfo_get_FileLength(finfo));
	if (code != HTTP_OK) {
		err_code = code;
		goto error_handler;
	}
	if (req->method == HTTPMETHOD_POST) {
		*rtype = RESP_POST;
		err_code = HTTP_OK;
		goto error_handler;
	}
	extra_headers = UpnpFileInfo_get_ExtraHeaders(finfo);
	if (!extra_headers) {
		extra_headers = "";
	}

	/* Check if chunked encoding should be used. */
	if (using_virtual_dir && UpnpFileInfo_get_FileLength(finfo) == UPNP_USING_CHUNKED) {
		/* Chunked encoding is only supported by HTTP 1.1 clients */
		if (resp_major == 1 && resp_minor == 1) {
			RespInstr->IsChunkActive = 1;
		} else {
			/* The virtual callback indicates that we should use
			 * chunked encoding however the client doesn't support
			 * it. Return with an internal server error. */
			err_code = HTTP_NOT_ACCEPTABLE;
			goto error_handler;
		}
	}

	aux_LastModified = UpnpFileInfo_get_LastModified(finfo);
	if (RespInstr->IsRangeActive && RespInstr->IsChunkActive) {
		/* Content-Range: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
		/* Transfer-Encoding: chunked */
		if (http_MakeMessage(headers, resp_major, resp_minor,
		    "R" "T" "GKLD" "s" "tcS" "Xc" "sCc",
		    HTTP_PARTIAL_CONTENT,	/* status code */
		    UpnpFileInfo_get_ContentType(finfo), /* content type */
		    RespInstr,	/* range info */
		    RespInstr,	/* language info */
		    "LAST-MODIFIED: ",
		    &aux_LastModified,
		    X_USER_AGENT, extra_headers) != 0) {
			goto error_handler;
		}
	} else if (RespInstr->IsRangeActive && !RespInstr->IsChunkActive) {
		/* Content-Range: bytes 222-3333/4000  HTTP_PARTIAL_CONTENT */
		if (http_MakeMessage(headers, resp_major, resp_minor,
		    "R" "N" "T" "GLD" "s" "tcS" "Xc" "sCc",
		    HTTP_PARTIAL_CONTENT,	/* status code */
		    RespInstr->ReadSendSize,	/* content length */
		    UpnpFileInfo_get_ContentType(finfo), /* content type */
		    RespInstr,	/* range info */
		    RespInstr,	/* language info */
		    "LAST-MODIFIED: ",
		    &aux_LastModified,
		    X_USER_AGENT, extra_headers) != 0) {
			goto error_handler;
		}
	} else if (!RespInstr->IsRangeActive && RespInstr->IsChunkActive) {
		/* Transfer-Encoding: chunked */
		if (http_MakeMessage(headers, resp_major, resp_minor,
		    "RK" "TLD" "s" "tcS" "Xc" "sCc",
		    HTTP_OK,	/* status code */
		    UpnpFileInfo_get_ContentType(finfo), /* content type */
		    RespInstr,	/* language info */
		    "LAST-MODIFIED: ",
		    &aux_LastModified,
		    X_USER_AGENT, extra_headers) != 0) {
			goto error_handler;
		}
	} else {
		/* !RespInstr->IsRangeActive && !RespInstr->IsChunkActive */
		if (RespInstr->ReadSendSize >= 0) {
			if (http_MakeMessage(headers, resp_major, resp_minor,
			    "R" "N" "TLD" "s" "tcS" "Xc" "sCc",
			    HTTP_OK,	/* status code */
			    RespInstr->ReadSendSize,	/* content length */
			    UpnpFileInfo_get_ContentType(finfo), /* content type */
			    RespInstr,	/* language info */
			    "LAST-MODIFIED: ",
			    &aux_LastModified,
			    X_USER_AGENT,
			    extra_headers) != 0) {
				goto error_handler;
			}
		} else {
			if (http_MakeMessage(headers, resp_major, resp_minor,
			    "R" "TLD" "s" "tcS" "Xc" "sCc",
			    HTTP_OK,	/* status code */
			    UpnpFileInfo_get_ContentType(finfo), /* content type */
			    RespInstr,	/* language info */
			    "LAST-MODIFIED: ",
			    &aux_LastModified,
			    X_USER_AGENT,
			    extra_headers) != 0) {
				goto error_handler;
			}
		}
	}
	if (req->method == HTTPMETHOD_HEAD) {
		*rtype = RESP_HEADERS;
	} else if (using_alias) {
		/* GET xml */
		*rtype = RESP_XMLDOC;
	} else if (using_virtual_dir) {
		*rtype = RESP_WEBDOC;
	} else {
		/* GET filename */
		*rtype = RESP_FILEDOC;
	}
	/* simple get http 0.9 as specified in http 1.0 */
	/* don't send headers */
	if (req->method == HTTPMETHOD_SIMPLEGET) {
		membuffer_destroy(headers);
	}
	err_code = HTTP_OK;

 error_handler:
	free(request_doc);
	UpnpFileInfo_delete(finfo);
	if (err_code != HTTP_OK && alias_grabbed) {
		alias_release(alias);
	}

	return err_code;
}
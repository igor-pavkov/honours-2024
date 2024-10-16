void web_server_callback(http_parser_t *parser, INOUT http_message_t *req,
	SOCKINFO *info)
{
	int ret;
	int timeout = -1;
	enum resp_type rtype = 0;
	membuffer headers;
	membuffer filename;
	struct xml_alias_t xmldoc;
	struct SendInstruction RespInstr;

	/*Initialize instruction header. */
	RespInstr.IsVirtualFile = 0;
	RespInstr.IsChunkActive = 0;
	RespInstr.IsRangeActive = 0;
	RespInstr.IsTrailers = 0;
	memset(RespInstr.AcceptLanguageHeader, 0,
	       sizeof(RespInstr.AcceptLanguageHeader));
	/* init */
	membuffer_init(&headers);
	membuffer_init(&filename);

	/*Process request should create the different kind of header depending on the */
	/*the type of request. */
	ret = process_request(req, &rtype, &headers, &filename, &xmldoc,
		&RespInstr);
	if (ret != HTTP_OK) {
		/* send error code */
		http_SendStatusResponse(info, ret, req->major_version,
			req->minor_version);
	} else {
		/* send response */
		switch (rtype) {
		case RESP_FILEDOC:
			http_SendMessage(info, &timeout, "Ibf",
					 &RespInstr,
					 headers.buf, headers.length,
					 filename.buf);
			break;
		case RESP_XMLDOC:
			http_SendMessage(info, &timeout, "Ibb",
				&RespInstr,
				headers.buf, headers.length,
				xmldoc.doc.buf, xmldoc.doc.length);
			alias_release(&xmldoc);
			break;
		case RESP_WEBDOC:
			/*http_SendVirtualDirDoc(info, &timeout, "Ibf",
				&RespInstr,
				headers.buf, headers.length,
				filename.buf);*/
			http_SendMessage(info, &timeout, "Ibf",
				&RespInstr,
				headers.buf, headers.length,
				filename.buf);
			break;
		case RESP_HEADERS:
			/* headers only */
			http_SendMessage(info, &timeout, "b",
				headers.buf, headers.length);
			break;
		case RESP_POST:
			/* headers only */
			ret = http_RecvPostMessage(parser, info, filename.buf,
				&RespInstr);
			/* Send response. */
			http_MakeMessage(&headers, 1, 1,
				"RTLSXcCc",
				ret, "text/html", &RespInstr, X_USER_AGENT);
			http_SendMessage(info, &timeout, "b",
				headers.buf, headers.length);
			break;
		default:
			UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
				"webserver: Invalid response type received.\n");
			assert(0);
		}
	}
	UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
		   "webserver: request processed...\n");
	membuffer_destroy(&headers);
	membuffer_destroy(&filename);
}
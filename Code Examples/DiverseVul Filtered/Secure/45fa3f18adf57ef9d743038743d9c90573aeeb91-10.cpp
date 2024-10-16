static void download_one_url(const char *url)
{
	bool use_proxy;                 /* Use proxies if env vars are set  */
	int redir_limit;
	len_and_sockaddr *lsa;
	FILE *sfp;                      /* socket to web/ftp server         */
	FILE *dfp;                      /* socket to ftp server (data)      */
	char *fname_out_alloc;
	char *redirected_path = NULL;
	struct host_info server;
	struct host_info target;

	server.allocated = NULL;
	target.allocated = NULL;
	server.user = NULL;
	target.user = NULL;

	parse_url(url, &target);

	/* Use the proxy if necessary */
	use_proxy = (strcmp(G.proxy_flag, "off") != 0);
	if (use_proxy) {
		char *proxy = getenv(target.protocol[0] == 'f' ? "ftp_proxy" : "http_proxy");
//FIXME: what if protocol is https? Ok to use http_proxy?
		use_proxy = (proxy && proxy[0]);
		if (use_proxy)
			parse_url(proxy, &server);
	}
	if (!use_proxy) {
		server.protocol = target.protocol;
		server.port = target.port;
		if (ENABLE_FEATURE_IPV6) {
			//free(server.allocated); - can't be non-NULL
			server.host = server.allocated = xstrdup(target.host);
		} else {
			server.host = target.host;
		}
	}

	if (ENABLE_FEATURE_IPV6)
		strip_ipv6_scope_id(target.host);

	/* If there was no -O FILE, guess output filename */
	fname_out_alloc = NULL;
	if (!(option_mask32 & WGET_OPT_OUTNAME)) {
		G.fname_out = bb_get_last_path_component_nostrip(target.path);
		/* handle "wget http://kernel.org//" */
		if (G.fname_out[0] == '/' || !G.fname_out[0])
			G.fname_out = (char*)"index.html";
		/* -P DIR is considered only if there was no -O FILE */
		if (G.dir_prefix)
			G.fname_out = fname_out_alloc = concat_path_file(G.dir_prefix, G.fname_out);
		else {
			/* redirects may free target.path later, need to make a copy */
			G.fname_out = fname_out_alloc = xstrdup(G.fname_out);
		}
	}
#if ENABLE_FEATURE_WGET_STATUSBAR
	G.curfile = bb_get_last_path_component_nostrip(G.fname_out);
#endif

	/* Determine where to start transfer */
	G.beg_range = 0;
	if (option_mask32 & WGET_OPT_CONTINUE) {
		G.output_fd = open(G.fname_out, O_WRONLY);
		if (G.output_fd >= 0) {
			G.beg_range = xlseek(G.output_fd, 0, SEEK_END);
		}
		/* File doesn't exist. We do not create file here yet.
		 * We are not sure it exists on remote side */
	}

	redir_limit = 16;
 resolve_lsa:
	lsa = xhost2sockaddr(server.host, server.port);
	if (!(option_mask32 & WGET_OPT_QUIET)) {
		char *s = xmalloc_sockaddr2dotted(&lsa->u.sa);
		fprintf(stderr, "Connecting to %s (%s)\n", server.host, s);
		free(s);
	}
 establish_session:
	/*G.content_len = 0; - redundant, got_clen = 0 is enough */
	G.got_clen = 0;
	G.chunked = 0;
	if (use_proxy || target.protocol[0] != 'f' /*not ftp[s]*/) {
		/*
		 *  HTTP session
		 */
		char *str;
		int status;

		/* Open socket to http(s) server */
#if ENABLE_FEATURE_WGET_OPENSSL
		/* openssl (and maybe internal TLS) support is configured */
		if (server.protocol == P_HTTPS) {
			/* openssl-based helper
			 * Inconvenient API since we can't give it an open fd
			 */
			int fd = spawn_https_helper_openssl(server.host, server.port);
# if ENABLE_FEATURE_WGET_HTTPS
			if (fd < 0) { /* no openssl? try internal */
				sfp = open_socket(lsa);
				spawn_ssl_client(server.host, fileno(sfp), /*flags*/ 0);
				goto socket_opened;
			}
# else
			/* We don't check for exec("openssl") failure in this case */
# endif
			sfp = fdopen(fd, "r+");
			if (!sfp)
				bb_die_memory_exhausted();
			goto socket_opened;
		}
		sfp = open_socket(lsa);
 socket_opened:
#elif ENABLE_FEATURE_WGET_HTTPS
		/* Only internal TLS support is configured */
		sfp = open_socket(lsa);
		if (server.protocol == P_HTTPS)
			spawn_ssl_client(server.host, fileno(sfp), /*flags*/ 0);
#else
		/* ssl (https) support is not configured */
		sfp = open_socket(lsa);
#endif
		/* Send HTTP request */
		if (use_proxy) {
			SENDFMT(sfp, "GET %s://%s/%s HTTP/1.1\r\n",
				target.protocol, target.host,
				target.path);
		} else {
			SENDFMT(sfp, "%s /%s HTTP/1.1\r\n",
				(option_mask32 & WGET_OPT_POST_DATA) ? "POST" : "GET",
				target.path);
		}
		if (!USR_HEADER_HOST)
			SENDFMT(sfp, "Host: %s\r\n", target.host);
		if (!USR_HEADER_USER_AGENT)
			SENDFMT(sfp, "User-Agent: %s\r\n", G.user_agent);

		/* Ask server to close the connection as soon as we are done
		 * (IOW: we do not intend to send more requests)
		 */
		SENDFMT(sfp, "Connection: close\r\n");

#if ENABLE_FEATURE_WGET_AUTHENTICATION
		if (target.user && !USR_HEADER_AUTH) {
			SENDFMT(sfp, "Proxy-Authorization: Basic %s\r\n"+6,
				base64enc(target.user));
		}
		if (use_proxy && server.user && !USR_HEADER_PROXY_AUTH) {
			SENDFMT(sfp, "Proxy-Authorization: Basic %s\r\n",
				base64enc(server.user));
		}
#endif

		if (G.beg_range != 0 && !USR_HEADER_RANGE)
			SENDFMT(sfp, "Range: bytes=%"OFF_FMT"u-\r\n", G.beg_range);

#if ENABLE_FEATURE_WGET_LONG_OPTIONS
		if (G.extra_headers) {
			log_io(G.extra_headers);
			fputs(G.extra_headers, sfp);
		}

		if (option_mask32 & WGET_OPT_POST_DATA) {
			SENDFMT(sfp,
				"Content-Type: application/x-www-form-urlencoded\r\n"
				"Content-Length: %u\r\n"
				"\r\n"
				"%s",
				(int) strlen(G.post_data), G.post_data
			);
		} else
#endif
		{
			SENDFMT(sfp, "\r\n");
		}

		fflush(sfp);

/* Tried doing this unconditionally.
 * Cloudflare and nginx/1.11.5 are shocked to see SHUT_WR on non-HTTPS.
 */
#if SSL_SUPPORTED
		if (target.protocol == P_HTTPS) {
			/* If we use SSL helper, keeping our end of the socket open for writing
			 * makes our end (i.e. the same fd!) readable (EAGAIN instead of EOF)
			 * even after child closes its copy of the fd.
			 * This helps:
			 */
			shutdown(fileno(sfp), SHUT_WR);
		}
#endif

		/*
		 * Retrieve HTTP response line and check for "200" status code.
		 */
 read_response:
		fgets_trim_sanitize(sfp, "  %s\n");

		str = G.wget_buf;
		str = skip_non_whitespace(str);
		str = skip_whitespace(str);
		// FIXME: no error check
		// xatou wouldn't work: "200 OK"
		status = atoi(str);
		switch (status) {
		case 0:
		case 100:
			while (get_sanitized_hdr(sfp) != NULL)
				/* eat all remaining headers */;
			goto read_response;

		/* Success responses */
		case 200:
			/* fall through */
		case 201: /* 201 Created */
/* "The request has been fulfilled and resulted in a new resource being created" */
			/* Standard wget is reported to treat this as success */
			/* fall through */
		case 202: /* 202 Accepted */
/* "The request has been accepted for processing, but the processing has not been completed" */
			/* Treat as success: fall through */
		case 203: /* 203 Non-Authoritative Information */
/* "Use of this response code is not required and is only appropriate when the response would otherwise be 200 (OK)" */
			/* fall through */
		case 204: /* 204 No Content */
/*
Response 204 doesn't say "null file", it says "metadata
has changed but data didn't":

"10.2.5 204 No Content
The server has fulfilled the request but does not need to return
an entity-body, and might want to return updated metainformation.
The response MAY include new or updated metainformation in the form
of entity-headers, which if present SHOULD be associated with
the requested variant.

If the client is a user agent, it SHOULD NOT change its document
view from that which caused the request to be sent. This response
is primarily intended to allow input for actions to take place
without causing a change to the user agent's active document view,
although any new or updated metainformation SHOULD be applied
to the document currently in the user agent's active view.

The 204 response MUST NOT include a message-body, and thus
is always terminated by the first empty line after the header fields."

However, in real world it was observed that some web servers
(e.g. Boa/0.94.14rc21) simply use code 204 when file size is zero.
*/
			if (G.beg_range != 0) {
				/* "Range:..." was not honored by the server.
				 * Restart download from the beginning.
				 */
				reset_beg_range_to_zero();
			}
			break;
		/* 205 Reset Content ?? what to do on this ?? 	*/

		case 300:  /* redirection */
		case 301:
		case 302:
		case 303:
			break;

		case 206: /* Partial Content */
			if (G.beg_range != 0)
				/* "Range:..." worked. Good. */
				break;
			/* Partial Content even though we did not ask for it??? */
			/* fall through */
		default:
			bb_error_msg_and_die("server returned error: %s", G.wget_buf);
		}

		/*
		 * Retrieve HTTP headers.
		 */
		while ((str = get_sanitized_hdr(sfp)) != NULL) {
			static const char keywords[] ALIGN1 =
				"content-length\0""transfer-encoding\0""location\0";
			enum {
				KEY_content_length = 1, KEY_transfer_encoding, KEY_location
			};
			smalluint key;

			/* get_sanitized_hdr converted "FOO:" string to lowercase */

			/* strip trailing whitespace */
			char *s = strchrnul(str, '\0') - 1;
			while (s >= str && (*s == ' ' || *s == '\t')) {
				*s = '\0';
				s--;
			}
			key = index_in_strings(keywords, G.wget_buf) + 1;
			if (key == KEY_content_length) {
				G.content_len = BB_STRTOOFF(str, NULL, 10);
				if (G.content_len < 0 || errno) {
					bb_error_msg_and_die("content-length %s is garbage", str);
				}
				G.got_clen = 1;
				continue;
			}
			if (key == KEY_transfer_encoding) {
				if (strcmp(str_tolower(str), "chunked") != 0)
					bb_error_msg_and_die("transfer encoding '%s' is not supported", str);
				G.chunked = 1;
			}
			if (key == KEY_location && status >= 300) {
				if (--redir_limit == 0)
					bb_simple_error_msg_and_die("too many redirections");
				fclose(sfp);
				if (str[0] == '/') {
					free(redirected_path);
					target.path = redirected_path = xstrdup(str + 1);
					/* lsa stays the same: it's on the same server */
				} else {
					parse_url(str, &target);
					if (!use_proxy) {
						/* server.user remains untouched */
						free(server.allocated);
						server.allocated = NULL;
						server.protocol = target.protocol;
						server.host = target.host;
						/* strip_ipv6_scope_id(target.host); - no! */
						/* we assume remote never gives us IPv6 addr with scope id */
						server.port = target.port;
						free(lsa);
						goto resolve_lsa;
					} /* else: lsa stays the same: we use proxy */
				}
				goto establish_session;
			}
		}
//		if (status >= 300)
//			bb_error_msg_and_die("bad redirection (no Location: header from server)");

		/* For HTTP, data is pumped over the same connection */
		dfp = sfp;
	} else {
		/*
		 *  FTP session
		 */
		sfp = prepare_ftp_session(&dfp, &target, lsa);
	}

	free(lsa);

	if (!(option_mask32 & WGET_OPT_SPIDER)) {
		if (G.output_fd < 0)
			G.output_fd = xopen(G.fname_out, G.o_flags);
		retrieve_file_data(dfp);
		if (!(option_mask32 & WGET_OPT_OUTNAME)) {
			xclose(G.output_fd);
			G.output_fd = -1;
		}
	} else {
		if (!(option_mask32 & WGET_OPT_QUIET))
			fprintf(stderr, "remote file exists\n");
	}

	if (dfp != sfp) {
		/* It's ftp. Close data connection properly */
		fclose(dfp);
		if (ftpcmd(NULL, NULL, sfp) != 226)
			bb_error_msg_and_die("ftp error: %s", G.wget_buf);
		/* ftpcmd("QUIT", NULL, sfp); - why bother? */
	}
	fclose(sfp);

	free(server.allocated);
	free(target.allocated);
	free(server.user);
	free(target.user);
	free(fname_out_alloc);
	free(redirected_path);
}
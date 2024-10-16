static int php_apache_request_ctor(request_rec *r, php_struct *ctx TSRMLS_DC)
{
	char *content_length;
	const char *auth;

	SG(sapi_headers).http_response_code = !r->status ? HTTP_OK : r->status;
	SG(request_info).content_type = apr_table_get(r->headers_in, "Content-Type");
	SG(request_info).query_string = apr_pstrdup(r->pool, r->args);
	SG(request_info).request_method = r->method;
	SG(request_info).proto_num = r->proto_num;
	SG(request_info).request_uri = apr_pstrdup(r->pool, r->uri);
	SG(request_info).path_translated = apr_pstrdup(r->pool, r->filename);
	r->no_local_copy = 1;

	content_length = (char *) apr_table_get(r->headers_in, "Content-Length");
	SG(request_info).content_length = (content_length ? atol(content_length) : 0);

	apr_table_unset(r->headers_out, "Content-Length");
	apr_table_unset(r->headers_out, "Last-Modified");
	apr_table_unset(r->headers_out, "Expires");
	apr_table_unset(r->headers_out, "ETag");

	auth = apr_table_get(r->headers_in, "Authorization");
	php_handle_auth_data(auth TSRMLS_CC);

	if (SG(request_info).auth_user == NULL && r->user) {
		SG(request_info).auth_user = estrdup(r->user);
	}

	ctx->r->user = apr_pstrdup(ctx->r->pool, SG(request_info).auth_user);

	return php_request_startup(TSRMLS_C);
}
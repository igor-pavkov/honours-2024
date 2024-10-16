static void strip_headers_request_body(request_rec *rnew)
{
    apr_table_unset(rnew->headers_in, "Content-Encoding");
    apr_table_unset(rnew->headers_in, "Content-Language");
    apr_table_unset(rnew->headers_in, "Content-Length");
    apr_table_unset(rnew->headers_in, "Content-Location");
    apr_table_unset(rnew->headers_in, "Content-MD5");
    apr_table_unset(rnew->headers_in, "Content-Range");
    apr_table_unset(rnew->headers_in, "Content-Type");
    apr_table_unset(rnew->headers_in, "Expires");
    apr_table_unset(rnew->headers_in, "Last-Modified");
    apr_table_unset(rnew->headers_in, "Transfer-Encoding");
}
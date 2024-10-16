static req_table_t* req_err_headers_out(request_rec *r)
{
  req_table_t* t = apr_palloc(r->pool, sizeof(req_table_t));
  t->r = r;
  t->t = r->err_headers_out;
  t->n = "err_headers_out";
  return t;
}
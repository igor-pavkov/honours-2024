static req_table_t* req_headers_out(request_rec *r)
{
  req_table_t* t = apr_palloc(r->pool, sizeof(req_table_t));
  t->r = r;
  t->t = r->headers_out;
  t->n = "headers_out";
  return t;
}
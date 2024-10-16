static req_table_t* req_headers_in(request_rec *r)
{
  req_table_t* t = apr_palloc(r->pool, sizeof(req_table_t));
  t->r = r;
  t->t = r->headers_in;
  t->n = "headers_in";
  return t;
}
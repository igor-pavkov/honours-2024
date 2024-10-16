static req_table_t* req_subprocess_env(request_rec *r)
{
  req_table_t* t = apr_palloc(r->pool, sizeof(req_table_t));
  t->r = r;
  t->t = r->subprocess_env;
  t->n = "subprocess_env";
  return t;
}
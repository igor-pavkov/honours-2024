static req_table_t* req_notes(request_rec *r)
{
  req_table_t* t = apr_palloc(r->pool, sizeof(req_table_t));
  t->r = r;
  t->t = r->notes;
  t->n = "notes";
  return t;
}
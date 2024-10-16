extern "C" int handler_rowid_filter_is_active(void *h_arg)
{
  if (!h_arg)
    return false;
  handler *h= (handler*) h_arg;
  return h->rowid_filter_is_active;
}
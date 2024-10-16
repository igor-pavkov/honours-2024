struct iri *iri_dup (const struct iri *src)
{
  struct iri *i = xmalloc (sizeof *i);
  i->uri_encoding = src->uri_encoding ? xstrdup (src->uri_encoding) : NULL;
  i->content_encoding = (src->content_encoding ?
                         xstrdup (src->content_encoding) : NULL);
  i->orig_url = src->orig_url ? xstrdup (src->orig_url) : NULL;
  i->utf8_encode = src->utf8_encode;
  return i;
}
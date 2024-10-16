static void freecookie(struct Cookie *co)
{
  if(co->expirestr)
    free(co->expirestr);
  if(co->domain)
    free(co->domain);
  if(co->path)
    free(co->path);
  if(co->name)
    free(co->name);
  if(co->value)
    free(co->value);
  if(co->maxage)
    free(co->maxage);
  if(co->version)
    free(co->version);

  free(co);
}
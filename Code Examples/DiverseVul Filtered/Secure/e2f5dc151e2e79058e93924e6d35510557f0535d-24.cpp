readconf_depends(driver_instance *d, uschar *s)
{
int count = *(d->info->options_count);
optionlist *ol;
uschar *ss;

for (ol = d->info->options; ol < d->info->options + count; ol++)
  {
  void *options_block;
  uschar *value;
  int type = ol->type & opt_mask;
  if (type != opt_stringptr) continue;
  options_block = ((ol->type & opt_public) == 0)? d->options_block : (void *)d;
  value = *(uschar **)((uschar *)options_block + (long int)(ol->value));
  if (value != NULL && (ss = Ustrstr(value, s)) != NULL)
    {
    if (ss <= value || (ss[-1] != '$' && ss[-1] != '{') ||
      isalnum(ss[Ustrlen(s)])) continue;
    DEBUG(D_transport) debug_printf("driver %s: \"%s\" option depends on %s\n",
      d->name, ol->name, s);
    return TRUE;
    }
  }

DEBUG(D_transport) debug_printf("driver %s does not depend on %s\n", d->name, s);
return FALSE;
}
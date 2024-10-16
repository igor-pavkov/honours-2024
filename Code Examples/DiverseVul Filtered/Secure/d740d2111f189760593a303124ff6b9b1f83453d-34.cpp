print_dsn_diagnostic_code(const address_item *addr, FILE *f)
{
uschar *s = testflag(addr, af_pass_message) ? addr->message : NULL;

/* af_pass_message and addr->message set ? print remote host answer */
if (s)
  {
  DEBUG(D_deliver)
    debug_printf("DSN Diagnostic-Code: addr->message = %s\n", addr->message);

  /* search first ": ". we assume to find the remote-MTA answer there */
  if (!(s = Ustrstr(addr->message, ": ")))
    return;				/* not found, bail out */
  s += 2;  /* skip ": " */
  fprintf(f, "Diagnostic-Code: smtp; ");
  }
/* no message available. do nothing */
else return;

while (*s)
  if (*s == '\\' && s[1] == 'n')
    {
    fputs("\n ", f);    /* as defined in RFC 3461 */
    s += 2;
    }
  else
    fputc(*s++, f);

fputc('\n', f);
}
deliver_split_address(address_item * addr)
{
uschar * address = addr->address;
uschar * domain;
uschar * t;
int len;

if (!(domain = Ustrrchr(address, '@')))
  return DEFER;		/* should always have a domain, but just in case... */

len = domain - address;
addr->domain = string_copylc(domain+1);    /* Domains are always caseless */

/* The implication in the RFCs (though I can't say I've seen it spelled out
explicitly) is that quoting should be removed from local parts at the point
where they are locally interpreted. [The new draft "821" is more explicit on
this, Jan 1999.] We know the syntax is valid, so this can be done by simply
removing quoting backslashes and any unquoted doublequotes. */

t = addr->cc_local_part = store_get(len+1);
while(len-- > 0)
  {
  int c = *address++;
  if (c == '\"') continue;
  if (c == '\\')
    {
    *t++ = *address++;
    len--;
    }
  else *t++ = c;
  }
*t = 0;

/* We do the percent hack only for those domains that are listed in
percent_hack_domains. A loop is required, to copy with multiple %-hacks. */

if (percent_hack_domains)
  {
  int rc;
  uschar *new_address = NULL;
  uschar *local_part = addr->cc_local_part;

  deliver_domain = addr->domain;  /* set $domain */

  while (  (rc = match_isinlist(deliver_domain, (const uschar **)&percent_hack_domains, 0,
	       &domainlist_anchor, addr->domain_cache, MCL_DOMAIN, TRUE, NULL))
             == OK
	&& (t = Ustrrchr(local_part, '%')) != NULL
	)
    {
    new_address = string_copy(local_part);
    new_address[t - local_part] = '@';
    deliver_domain = string_copylc(t+1);
    local_part = string_copyn(local_part, t - local_part);
    }

  if (rc == DEFER) return DEFER;   /* lookup deferred */

  /* If hackery happened, set up new parent and alter the current address. */

  if (new_address)
    {
    address_item *new_parent = store_get(sizeof(address_item));
    *new_parent = *addr;
    addr->parent = new_parent;
    new_parent->child_count = 1;
    addr->address = new_address;
    addr->unique = string_copy(new_address);
    addr->domain = deliver_domain;
    addr->cc_local_part = local_part;
    DEBUG(D_deliver) debug_printf("%%-hack changed address to: %s\n",
      addr->address);
    }
  }

/* Create the lowercased version of the final local part, and make that the
default one to be used. */

addr->local_part = addr->lc_local_part = string_copylc(addr->cc_local_part);
return OK;
}
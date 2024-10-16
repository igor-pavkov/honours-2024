remote_post_process(address_item *addr, int logflags, uschar *msg,
  BOOL fallback)
{
host_item *h;

/* If any host addresses were found to be unusable, add them to the unusable
tree so that subsequent deliveries don't try them. */

for (h = addr->host_list; h; h = h->next)
  if (h->address)
    if (h->status >= hstatus_unusable) tree_add_unusable(h);

/* Now handle each address on the chain. The transport has placed '=' or '-'
into the special_action field for each successful delivery. */

while (addr)
  {
  address_item *next = addr->next;

  /* If msg == NULL (normal processing) and the result is DEFER and we are
  processing the main hosts and there are fallback hosts available, put the
  address on the list for fallback delivery. */

  if (  addr->transport_return == DEFER
     && addr->fallback_hosts
     && !fallback
     && !msg
     )
    {
    addr->host_list = addr->fallback_hosts;
    addr->next = addr_fallback;
    addr_fallback = addr;
    DEBUG(D_deliver) debug_printf("%s queued for fallback host(s)\n", addr->address);
    }

  /* If msg is set (=> unexpected problem), set it in the address before
  doing the ordinary post processing. */

  else
    {
    if (msg)
      {
      addr->message = msg;
      addr->transport_return = DEFER;
      }
    (void)post_process_one(addr, addr->transport_return, logflags,
      EXIM_DTYPE_TRANSPORT, addr->special_action);
    }

  /* Next address */

  addr = next;
  }

/* If we have just delivered down a passed SMTP channel, and that was
the last address, the channel will have been closed down. Now that
we have logged that delivery, set continue_sequence to 1 so that
any subsequent deliveries don't get "*" incorrectly logged. */

if (!continue_transport) continue_sequence = 1;
}
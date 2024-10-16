previously_transported(address_item *addr, BOOL testing)
{
(void)string_format(big_buffer, big_buffer_size, "%s/%s",
  addr->unique + (testflag(addr, af_homonym)? 3:0), addr->transport->name);

if (tree_search(tree_nonrecipients, big_buffer) != 0)
  {
  DEBUG(D_deliver|D_route|D_transport)
    debug_printf("%s was previously delivered (%s transport): discarded\n",
    addr->address, addr->transport->name);
  if (!testing) child_done(addr, tod_stamp(tod_log));
  return TRUE;
  }

return FALSE;
}
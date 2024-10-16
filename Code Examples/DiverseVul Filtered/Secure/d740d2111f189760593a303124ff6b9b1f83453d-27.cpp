do_duplicate_check(address_item **anchor)
{
address_item *addr;
while ((addr = *anchor))
  {
  tree_node *tnode;
  if (testflag(addr, af_pfr))
    {
    anchor = &(addr->next);
    }
  else if ((tnode = tree_search(tree_duplicates, addr->unique)))
    {
    DEBUG(D_deliver|D_route)
      debug_printf("%s is a duplicate address: discarded\n", addr->unique);
    *anchor = addr->next;
    addr->dupof = tnode->data.ptr;
    addr->next = addr_duplicate;
    addr_duplicate = addr;
    }
  else
    {
    tree_add_duplicate(addr->unique, addr);
    anchor = &(addr->next);
    }
  }
}
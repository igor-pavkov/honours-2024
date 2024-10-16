handler *handler::clone(const char *name, MEM_ROOT *mem_root)
{
  handler *new_handler= get_new_handler(table->s, mem_root, ht);

  if (!new_handler)
    return NULL;
  if (new_handler->set_ha_share_ref(ha_share))
    goto err;

  /*
    TODO: Implement a more efficient way to have more than one index open for
    the same table instance. The ha_open call is not cacheable for clone.

    This is not critical as the engines already have the table open
    and should be able to use the original instance of the table.
  */
  if (new_handler->ha_open(table, name, table->db_stat,
                           HA_OPEN_IGNORE_IF_LOCKED, mem_root))
    goto err;

  return new_handler;

err:
  delete new_handler;
  return NULL;
}
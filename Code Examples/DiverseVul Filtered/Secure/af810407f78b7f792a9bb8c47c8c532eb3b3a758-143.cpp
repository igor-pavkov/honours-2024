handler::ha_delete_table(const char *name)
{
  mark_trx_read_write();
  return delete_table(name);
}
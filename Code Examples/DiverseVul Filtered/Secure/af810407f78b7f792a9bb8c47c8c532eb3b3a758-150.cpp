handler::ha_drop_partitions(const char *path)
{
  DBUG_ASSERT(!table->db_stat);

  mark_trx_read_write();

  return drop_partitions(path);
}
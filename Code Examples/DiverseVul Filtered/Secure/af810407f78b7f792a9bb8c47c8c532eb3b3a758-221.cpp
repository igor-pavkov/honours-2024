handler::ha_rename_partitions(const char *path)
{
  DBUG_ASSERT(!table->db_stat);

  mark_trx_read_write();

  return rename_partitions(path);
}
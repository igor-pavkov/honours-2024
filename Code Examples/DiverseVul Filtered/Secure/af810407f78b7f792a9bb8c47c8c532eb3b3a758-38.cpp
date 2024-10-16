handler::ha_rename_table(const char *from, const char *to)
{
  DBUG_ASSERT(m_lock_type == F_UNLCK);
  mark_trx_read_write();

  return rename_table(from, to);
}
handler::ha_drop_table(const char *name)
{
  DBUG_ASSERT(m_lock_type == F_UNLCK);
  mark_trx_read_write();

  return drop_table(name);
}
handler::ha_create_partitioning_metadata(const char *name,
                                         const char *old_name,
                                         int action_flag)
{
  /*
    Normally this is done when unlocked, but in fast_alter_partition_table,
    it is done on an already locked handler when preparing to alter/rename
    partitions.
  */
  DBUG_ASSERT(m_lock_type == F_UNLCK ||
              (!old_name && strcmp(name, table_share->path.str)));


  mark_trx_read_write();
  return create_partitioning_metadata(name, old_name, action_flag);
}
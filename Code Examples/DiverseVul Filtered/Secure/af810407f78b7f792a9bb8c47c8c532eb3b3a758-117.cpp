handler::ha_change_partitions(HA_CREATE_INFO *create_info,
                              const char *path,
                              ulonglong * const copied,
                              ulonglong * const deleted,
                              const uchar *pack_frm_data,
                              size_t pack_frm_len)
{
  /*
    Must have at least RDLCK or be a TMP table. Read lock is needed to read
    from current partitions and write lock will be taken on new partitions.
  */
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);

  mark_trx_read_write();

  return change_partitions(create_info, path, copied, deleted,
                           pack_frm_data, pack_frm_len);
}
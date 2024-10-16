bool handler::ha_prepare_inplace_alter_table(TABLE *altered_table,
                                             Alter_inplace_info *ha_alter_info)
{
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              m_lock_type != F_UNLCK);
  mark_trx_read_write();

  return prepare_inplace_alter_table(altered_table, ha_alter_info);
}
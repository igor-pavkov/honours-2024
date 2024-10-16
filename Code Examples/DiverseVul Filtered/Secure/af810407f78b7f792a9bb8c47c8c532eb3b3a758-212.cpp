void handler::unlock_shared_ha_data()
{
  DBUG_ASSERT(table_share);
  if (table_share->tmp_table == NO_TMP_TABLE)
    mysql_mutex_unlock(&table_share->LOCK_ha_data);
}
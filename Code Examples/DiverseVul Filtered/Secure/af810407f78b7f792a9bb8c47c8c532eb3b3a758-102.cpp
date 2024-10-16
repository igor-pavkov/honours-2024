Handler_share *handler::get_ha_share_ptr()
{
  DBUG_ENTER("handler::get_ha_share_ptr");
  DBUG_ASSERT(ha_share);
  DBUG_ASSERT(table_share);

#ifndef DBUG_OFF
  if (table_share->tmp_table == NO_TMP_TABLE)
    mysql_mutex_assert_owner(&table_share->LOCK_ha_data);
#endif

  DBUG_RETURN(*ha_share);
}
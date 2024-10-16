void handler::set_ha_share_ptr(Handler_share *arg_ha_share)
{
  DBUG_ENTER("handler::set_ha_share_ptr");
  DBUG_ASSERT(ha_share);
#ifndef DBUG_OFF
  if (table_share->tmp_table == NO_TMP_TABLE)
    mysql_mutex_assert_owner(&table_share->LOCK_ha_data);
#endif

  *ha_share= arg_ha_share;
  DBUG_VOID_RETURN;
}
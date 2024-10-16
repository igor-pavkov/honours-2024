int handler::ha_repair(THD* thd, HA_CHECK_OPT* check_opt)
{
  int result;

  mark_trx_read_write();

  result= repair(thd, check_opt);
  DBUG_ASSERT(result == HA_ADMIN_NOT_IMPLEMENTED ||
              ha_table_flags() & HA_CAN_REPAIR);

  if (result == HA_ADMIN_OK)
    result= update_frm_version(table);
  return result;
}
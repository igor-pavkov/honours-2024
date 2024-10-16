int ha_prepare(THD *thd)
{
  int error=0, all=1;
  THD_TRANS *trans=all ? &thd->transaction.all : &thd->transaction.stmt;
  Ha_trx_info *ha_info= trans->ha_list;
  DBUG_ENTER("ha_prepare");

  if (ha_info)
  {
    for (; ha_info; ha_info= ha_info->next())
    {
      handlerton *ht= ha_info->ht();
      if (ht->prepare)
      {
        if (unlikely(prepare_or_error(ht, thd, all)))
        {
          ha_rollback_trans(thd, all);
          error=1;
          break;
        }
      }
      else
      {
        push_warning_printf(thd, Sql_condition::WARN_LEVEL_WARN,
                            ER_GET_ERRNO, ER_THD(thd, ER_GET_ERRNO),
                            HA_ERR_WRONG_COMMAND,
                            ha_resolve_storage_engine_name(ht));

      }
    }
  }

  DBUG_RETURN(error);
}
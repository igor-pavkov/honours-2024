bool ha_rollback_to_savepoint_can_release_mdl(THD *thd)
{
  Ha_trx_info *ha_info;
  THD_TRANS *trans= (thd->in_sub_stmt ? &thd->transaction.stmt :
                                        &thd->transaction.all);

  DBUG_ENTER("ha_rollback_to_savepoint_can_release_mdl");

  /**
    Checking whether it is safe to release metadata locks after rollback to
    savepoint in all the storage engines that are part of the transaction.
  */
  for (ha_info= trans->ha_list; ha_info; ha_info= ha_info->next())
  {
    handlerton *ht= ha_info->ht();
    DBUG_ASSERT(ht);

    if (ht->savepoint_rollback_can_release_mdl == 0 ||
        ht->savepoint_rollback_can_release_mdl(ht, thd) == false)
      DBUG_RETURN(false);
  }

  DBUG_RETURN(true);
}
int ha_rollback_to_savepoint(THD *thd, SAVEPOINT *sv)
{
  int error=0;
  THD_TRANS *trans= (thd->in_sub_stmt ? &thd->transaction.stmt :
                                        &thd->transaction.all);
  Ha_trx_info *ha_info, *ha_info_next;

  DBUG_ENTER("ha_rollback_to_savepoint");

  trans->no_2pc=0;
  /*
    rolling back to savepoint in all storage engines that were part of the
    transaction when the savepoint was set
  */
  for (ha_info= sv->ha_list; ha_info; ha_info= ha_info->next())
  {
    int err;
    handlerton *ht= ha_info->ht();
    DBUG_ASSERT(ht);
    DBUG_ASSERT(ht->savepoint_set != 0);
    if ((err= ht->savepoint_rollback(ht, thd,
                                     (uchar *)(sv+1)+ht->savepoint_offset)))
    { // cannot happen
      my_error(ER_ERROR_DURING_ROLLBACK, MYF(0), err);
      error=1;
    }
    status_var_increment(thd->status_var.ha_savepoint_rollback_count);
    trans->no_2pc|= ht->prepare == 0;
  }
  /*
    rolling back the transaction in all storage engines that were not part of
    the transaction when the savepoint was set
  */
  for (ha_info= trans->ha_list; ha_info != sv->ha_list;
       ha_info= ha_info_next)
  {
    int err;
    handlerton *ht= ha_info->ht();
#ifdef WITH_WSREP
    if (WSREP(thd) && ht->flags & HTON_WSREP_REPLICATION)
    {
      WSREP_DEBUG("ha_rollback_to_savepoint: run before_rollbackha_rollback_trans hook");
      (void) wsrep_before_rollback(thd, !thd->in_sub_stmt);

    }
#endif // WITH_WSREP
    if ((err= ht->rollback(ht, thd, !thd->in_sub_stmt)))
    { // cannot happen
      my_error(ER_ERROR_DURING_ROLLBACK, MYF(0), err);
      error=1;
    }
#ifdef WITH_WSREP
    if (WSREP(thd) && ht->flags & HTON_WSREP_REPLICATION)
    {
      WSREP_DEBUG("ha_rollback_to_savepoint: run after_rollback hook");
      (void) wsrep_after_rollback(thd, !thd->in_sub_stmt);
    }
#endif // WITH_WSREP
    status_var_increment(thd->status_var.ha_rollback_count);
    ha_info_next= ha_info->next();
    ha_info->reset(); /* keep it conveniently zero-filled */
  }
  trans->ha_list= sv->ha_list;
  DBUG_RETURN(error);
}
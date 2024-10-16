int ha_rollback_trans(THD *thd, bool all)
{
  int error=0;
  THD_TRANS *trans=all ? &thd->transaction.all : &thd->transaction.stmt;
  Ha_trx_info *ha_info= trans->ha_list, *ha_info_next;
  /*
    "real" is a nick name for a transaction for which a commit will
    make persistent changes. E.g. a 'stmt' transaction inside a 'all'
    transaction is not 'real': even though it's possible to commit it,
    the changes are not durable as they might be rolled back if the
    enclosing 'all' transaction is rolled back.
    We establish the value of 'is_real_trans' by checking
    if it's an explicit COMMIT or BEGIN statement, or implicit
    commit issued by DDL (in these cases all == TRUE),
    or if we're running in autocommit mode (it's only in the autocommit mode
    ha_commit_one_phase() is called with an empty
    transaction.all.ha_list, see why in trans_register_ha()).
  */
  bool is_real_trans=all || thd->transaction.all.ha_list == 0;
  DBUG_ENTER("ha_rollback_trans");

  /*
    We must not rollback the normal transaction if a statement
    transaction is pending.
  */
  DBUG_ASSERT(thd->transaction.stmt.ha_list == NULL ||
              trans == &thd->transaction.stmt);

#ifdef HAVE_REPLICATION
  if (is_real_trans)
  {
    /*
      In parallel replication, if we need to rollback during commit, we must
      first inform following transactions that we are going to abort our commit
      attempt. Otherwise those following transactions can run too early, and
      possibly cause replication to fail. See comments in retry_event_group().

      There were several bugs with this in the past that were very hard to
      track down (MDEV-7458, MDEV-8302). So we add here an assertion for
      rollback without signalling following transactions. And in release
      builds, we explicitly do the signalling before rolling back.
    */
    DBUG_ASSERT(!(thd->rgi_slave && thd->rgi_slave->did_mark_start_commit));
    if (thd->rgi_slave && thd->rgi_slave->did_mark_start_commit)
      thd->rgi_slave->unmark_start_commit();
  }
#endif

  if (thd->in_sub_stmt)
  {
    DBUG_ASSERT(0);
    /*
      If we are inside stored function or trigger we should not commit or
      rollback current statement transaction. See comment in ha_commit_trans()
      call for more information.
    */
    if (!all)
      DBUG_RETURN(0);
    my_error(ER_COMMIT_NOT_ALLOWED_IN_SF_OR_TRG, MYF(0));
    DBUG_RETURN(1);
  }

#ifdef WITH_WSREP
  (void) wsrep_before_rollback(thd, all);
#endif /* WITH_WSREP */
  if (ha_info)
  {
    /* Close all cursors that can not survive ROLLBACK */
    if (is_real_trans)                          /* not a statement commit */
      thd->stmt_map.close_transient_cursors();

    for (; ha_info; ha_info= ha_info_next)
    {
      int err;
      handlerton *ht= ha_info->ht();
      if ((err= ht->rollback(ht, thd, all)))
      { // cannot happen
        my_error(ER_ERROR_DURING_ROLLBACK, MYF(0), err);
        error=1;
#ifdef WITH_WSREP
        WSREP_WARN("handlerton rollback failed, thd %lld %lld conf %d SQL %s",
                   thd->thread_id, thd->query_id, thd->wsrep_trx().state(),
                   thd->query());
#endif /* WITH_WSREP */
      }
      status_var_increment(thd->status_var.ha_rollback_count);
      ha_info_next= ha_info->next();
      ha_info->reset(); /* keep it conveniently zero-filled */
    }
    trans->ha_list= 0;
    trans->no_2pc=0;
  }

#ifdef WITH_WSREP
  if (thd->is_error())
  {
    WSREP_DEBUG("ha_rollback_trans(%lld, %s) rolled back: %s: %s; is_real %d",
                thd->thread_id, all?"TRUE":"FALSE", wsrep_thd_query(thd),
                thd->get_stmt_da()->message(), is_real_trans);
  }
  (void) wsrep_after_rollback(thd, all);
#endif /* WITH_WSREP */
  /* Always cleanup. Even if nht==0. There may be savepoints. */
  if (is_real_trans)
  {
    /*
      Thanks to possibility of MDL deadlock rollback request can come even if
      transaction hasn't been started in any transactional storage engine.
    */
    if (thd->transaction_rollback_request &&
        thd->transaction.xid_state.is_explicit_XA())
      thd->transaction.xid_state.set_error(thd->get_stmt_da()->sql_errno());

    thd->has_waiter= false;
    thd->transaction.cleanup();
  }
  if (all)
    thd->transaction_rollback_request= FALSE;

  /*
    If a non-transactional table was updated, warn; don't warn if this is a
    slave thread (because when a slave thread executes a ROLLBACK, it has
    been read from the binary log, so it's 100% sure and normal to produce
    error ER_WARNING_NOT_COMPLETE_ROLLBACK. If we sent the warning to the
    slave SQL thread, it would not stop the thread but just be printed in
    the error log; but we don't want users to wonder why they have this
    message in the error log, so we don't send it.

    We don't have to test for thd->killed == KILL_SYSTEM_THREAD as
    it doesn't matter if a warning is pushed to a system thread or not:
    No one will see it...
  */
  if (is_real_trans && thd->transaction.all.modified_non_trans_table &&
      !thd->slave_thread && thd->killed < KILL_CONNECTION)
    push_warning(thd, Sql_condition::WARN_LEVEL_WARN,
                 ER_WARNING_NOT_COMPLETE_ROLLBACK,
                 ER_THD(thd, ER_WARNING_NOT_COMPLETE_ROLLBACK));
#ifdef HAVE_REPLICATION
  repl_semisync_master.wait_after_rollback(thd, all);
#endif
  DBUG_RETURN(error);
}
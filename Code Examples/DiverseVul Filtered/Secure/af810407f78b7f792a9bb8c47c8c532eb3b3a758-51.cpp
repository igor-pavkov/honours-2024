int ha_commit_trans(THD *thd, bool all)
{
  int error= 0, cookie;
  /*
    'all' means that this is either an explicit commit issued by
    user, or an implicit commit issued by a DDL.
  */
  THD_TRANS *trans= all ? &thd->transaction.all : &thd->transaction.stmt;
  /*
    "real" is a nick name for a transaction for which a commit will
    make persistent changes. E.g. a 'stmt' transaction inside an 'all'
    transaction is not 'real': even though it's possible to commit it,
    the changes are not durable as they might be rolled back if the
    enclosing 'all' transaction is rolled back.
  */
  bool is_real_trans= ((all || thd->transaction.all.ha_list == 0) &&
                       !(thd->variables.option_bits & OPTION_GTID_BEGIN));
  Ha_trx_info *ha_info= trans->ha_list;
  bool need_prepare_ordered, need_commit_ordered;
  my_xid xid;
#ifdef WITH_WSREP
  const bool run_wsrep_hooks= wsrep_run_commit_hook(thd, all);
#endif /* WITH_WSREP */
  DBUG_ENTER("ha_commit_trans");
  DBUG_PRINT("info",("thd: %p  option_bits: %lu  all: %d",
                     thd, (ulong) thd->variables.option_bits, all));

  /* Just a random warning to test warnings pushed during autocommit. */
  DBUG_EXECUTE_IF("warn_during_ha_commit_trans",
    push_warning(thd, Sql_condition::WARN_LEVEL_WARN,
                 ER_WARNING_NOT_COMPLETE_ROLLBACK,
                 ER_THD(thd, ER_WARNING_NOT_COMPLETE_ROLLBACK)););

  DBUG_PRINT("info",
             ("all: %d  thd->in_sub_stmt: %d  ha_info: %p  is_real_trans: %d",
              all, thd->in_sub_stmt, ha_info, is_real_trans));
  /*
    We must not commit the normal transaction if a statement
    transaction is pending. Otherwise statement transaction
    flags will not get propagated to its normal transaction's
    counterpart.
  */
  DBUG_ASSERT(thd->transaction.stmt.ha_list == NULL ||
              trans == &thd->transaction.stmt);

  if (thd->in_sub_stmt)
  {
    DBUG_ASSERT(0);
    /*
      Since we don't support nested statement transactions in 5.0,
      we can't commit or rollback stmt transactions while we are inside
      stored functions or triggers. So we simply do nothing now.
      TODO: This should be fixed in later ( >= 5.1) releases.
    */
    if (!all)
      DBUG_RETURN(0);
    /*
      We assume that all statements which commit or rollback main transaction
      are prohibited inside of stored functions or triggers. So they should
      bail out with error even before ha_commit_trans() call. To be 100% safe
      let us throw error in non-debug builds.
    */
    my_error(ER_COMMIT_NOT_ALLOWED_IN_SF_OR_TRG, MYF(0));
    DBUG_RETURN(2);
  }

  if (!ha_info)
  {
    /*
      Free resources and perform other cleanup even for 'empty' transactions.
    */
    if (is_real_trans)
      thd->transaction.cleanup();
#ifdef WITH_WSREP
    if (wsrep_is_active(thd) && is_real_trans && !error)
    {
      wsrep_commit_empty(thd, all);
    }
#endif /* WITH_WSREP */

    ha_maria_implicit_commit(thd, TRUE);
    DBUG_RETURN(error);
  }

  DBUG_EXECUTE_IF("crash_commit_before", DBUG_SUICIDE(););

  /* Close all cursors that can not survive COMMIT */
  if (is_real_trans)                          /* not a statement commit */
    thd->stmt_map.close_transient_cursors();

  uint rw_ha_count= ha_check_and_coalesce_trx_read_only(thd, ha_info, all);
  /* rw_trans is TRUE when we in a transaction changing data */
  bool rw_trans= is_real_trans &&
                 (rw_ha_count > (thd->is_current_stmt_binlog_disabled()?0U:1U));
  MDL_request mdl_backup;
  DBUG_PRINT("info", ("is_real_trans: %d  rw_trans:  %d  rw_ha_count: %d",
                      is_real_trans, rw_trans, rw_ha_count));

  if (rw_trans)
  {
    /*
      Acquire a metadata lock which will ensure that COMMIT is blocked
      by an active FLUSH TABLES WITH READ LOCK (and vice versa:
      COMMIT in progress blocks FTWRL).

      We allow the owner of FTWRL to COMMIT; we assume that it knows
      what it does.
    */
    mdl_backup.init(MDL_key::BACKUP, "", "", MDL_BACKUP_COMMIT, MDL_EXPLICIT);

    if (!WSREP(thd))
    {
      if (thd->mdl_context.acquire_lock(&mdl_backup,
                                        thd->variables.lock_wait_timeout))
      {
        ha_rollback_trans(thd, all);
        DBUG_RETURN(1);
      }
      thd->backup_commit_lock= &mdl_backup;
    }
    DEBUG_SYNC(thd, "ha_commit_trans_after_acquire_commit_lock");

    /* Use shortcut as we already have the MDL_BACKUP_COMMIT lock */
    ha_maria::implicit_commit(thd, TRUE);
  }
  else
    ha_maria_implicit_commit(thd, TRUE);

  if (rw_trans &&
      opt_readonly &&
      !(thd->security_ctx->master_access & SUPER_ACL) &&
      !thd->slave_thread)
  {
    my_error(ER_OPTION_PREVENTS_STATEMENT, MYF(0), "--read-only");
    goto err;
  }

#if 1 // FIXME: This should be done in ha_prepare().
  if (rw_trans || (thd->lex->sql_command == SQLCOM_ALTER_TABLE &&
                   thd->lex->alter_info.flags & ALTER_ADD_SYSTEM_VERSIONING &&
                   is_real_trans))
  {
    ulonglong trx_start_id= 0, trx_end_id= 0;
    for (Ha_trx_info *ha_info= trans->ha_list; ha_info; ha_info= ha_info->next())
    {
      if (ha_info->ht()->prepare_commit_versioned)
      {
        trx_end_id= ha_info->ht()->prepare_commit_versioned(thd, &trx_start_id);
        if (trx_end_id)
          break; // FIXME: use a common ID for cross-engine transactions
      }
    }

    if (trx_end_id)
    {
      if (!TR_table::use_transaction_registry)
      {
        my_error(ER_VERS_TRT_IS_DISABLED, MYF(0));
        goto err;
      }
      DBUG_ASSERT(trx_start_id);
#ifdef WITH_WSREP
      bool saved_wsrep_on= thd->variables.wsrep_on;
      thd->variables.wsrep_on= false;
#endif
      TR_table trt(thd, true);
      if (trt.update(trx_start_id, trx_end_id))
      {
#ifdef WITH_WSREP
        thd->variables.wsrep_on= saved_wsrep_on;
#endif
        (void) trans_rollback_stmt(thd);
        goto err;
      }
      // Here, the call will not commit inside InnoDB. It is only working
      // around closing thd->transaction.stmt open by TR_table::open().
      if (all)
        commit_one_phase_2(thd, false, &thd->transaction.stmt, false);
#ifdef WITH_WSREP
      thd->variables.wsrep_on= saved_wsrep_on;
#endif
    }
  }
#endif

  if (trans->no_2pc || (rw_ha_count <= 1))
  {
#ifdef WITH_WSREP
    /*
      This commit will not go through log_and_order() where wsrep commit
      ordering is normally done. Commit ordering must be done here.
    */
    if (run_wsrep_hooks)
      error= wsrep_before_commit(thd, all);
    if (error)
    {
      ha_rollback_trans(thd, FALSE);
      goto wsrep_err;
    }
#endif /* WITH_WSREP */
    error= ha_commit_one_phase(thd, all);
#ifdef WITH_WSREP
    if (run_wsrep_hooks)
      error= error || wsrep_after_commit(thd, all);
#endif /* WITH_WSREP */
    goto done;
  }

  need_prepare_ordered= FALSE;
  need_commit_ordered= FALSE;
  DBUG_ASSERT(thd->transaction.implicit_xid.get_my_xid() ==
              thd->transaction.implicit_xid.quick_get_my_xid());
  xid= thd->transaction.xid_state.is_explicit_XA() ? 0 :
       thd->transaction.implicit_xid.quick_get_my_xid();

  for (Ha_trx_info *hi= ha_info; hi; hi= hi->next())
  {
    handlerton *ht= hi->ht();
    /*
      Do not call two-phase commit if this particular
      transaction is read-only. This allows for simpler
      implementation in engines that are always read-only.
    */
    if (! hi->is_trx_read_write())
      continue;
    /*
      Sic: we know that prepare() is not NULL since otherwise
      trans->no_2pc would have been set.
    */
    if (unlikely(prepare_or_error(ht, thd, all)))
      goto err;

    need_prepare_ordered|= (ht->prepare_ordered != NULL);
    need_commit_ordered|= (ht->commit_ordered != NULL);
  }
  DEBUG_SYNC(thd, "ha_commit_trans_after_prepare");
  DBUG_EXECUTE_IF("crash_commit_after_prepare", DBUG_SUICIDE(););

#ifdef WITH_WSREP
  if (run_wsrep_hooks && !error)
  {
    wsrep::seqno const s= wsrep_xid_seqno(thd->wsrep_xid);
    if (!s.is_undefined())
    {
      // xid was rewritten by wsrep
      xid= s.get();
    }
  }
#endif /* WITH_WSREP */

  if (!is_real_trans)
  {
    error= commit_one_phase_2(thd, all, trans, is_real_trans);
    goto done;
  }
#ifdef WITH_WSREP
  if (run_wsrep_hooks && (error = wsrep_before_commit(thd, all)))
    goto wsrep_err;
#endif /* WITH_WSREP */
  DEBUG_SYNC(thd, "ha_commit_trans_before_log_and_order");
  cookie= tc_log->log_and_order(thd, xid, all, need_prepare_ordered,
                                need_commit_ordered);
  if (!cookie)
  {
    WSREP_DEBUG("log_and_order has failed %llu %d", thd->thread_id, cookie);
    goto err;
  }
  DEBUG_SYNC(thd, "ha_commit_trans_after_log_and_order");
  DBUG_EXECUTE_IF("crash_commit_after_log", DBUG_SUICIDE(););

  error= commit_one_phase_2(thd, all, trans, is_real_trans) ? 2 : 0;
#ifdef WITH_WSREP
  if (run_wsrep_hooks && (error || (error = wsrep_after_commit(thd, all))))
  {
    mysql_mutex_lock(&thd->LOCK_thd_data);
    if (wsrep_must_abort(thd))
    {
      mysql_mutex_unlock(&thd->LOCK_thd_data);
      (void)tc_log->unlog(cookie, xid);
      goto wsrep_err;
    }
    mysql_mutex_unlock(&thd->LOCK_thd_data);
  }
#endif /* WITH_WSREP */
  DBUG_EXECUTE_IF("crash_commit_before_unlog", DBUG_SUICIDE(););
  if (tc_log->unlog(cookie, xid))
  {
    error= 2;                                /* Error during commit */
    goto end;
  }

done:
  DBUG_EXECUTE_IF("crash_commit_after", DBUG_SUICIDE(););

  mysql_mutex_assert_not_owner(&LOCK_prepare_ordered);
  mysql_mutex_assert_not_owner(mysql_bin_log.get_log_lock());
  mysql_mutex_assert_not_owner(&LOCK_after_binlog_sync);
  mysql_mutex_assert_not_owner(&LOCK_commit_ordered);
#ifdef HAVE_REPLICATION
  repl_semisync_master.wait_after_commit(thd, all);
  DEBUG_SYNC(thd, "after_group_after_commit");
#endif
  goto end;

  /* Come here if error and we need to rollback. */
#ifdef WITH_WSREP
wsrep_err:
  mysql_mutex_lock(&thd->LOCK_thd_data);
  if (run_wsrep_hooks && wsrep_must_abort(thd))
  {
    WSREP_DEBUG("BF abort has happened after prepare & certify");
    mysql_mutex_unlock(&thd->LOCK_thd_data);
    ha_rollback_trans(thd, TRUE);
  }
  else
    mysql_mutex_unlock(&thd->LOCK_thd_data);

#endif /* WITH_WSREP */
err:
  error= 1;                                  /* Transaction was rolled back */
  /*
    In parallel replication, rollback is delayed, as there is extra replication
    book-keeping to be done before rolling back and allowing a conflicting
    transaction to continue (MDEV-7458).
  */
  if (!(thd->rgi_slave && thd->rgi_slave->is_parallel_exec))
    ha_rollback_trans(thd, all);
  else
  {
    WSREP_DEBUG("rollback skipped %p %d",thd->rgi_slave,
                thd->rgi_slave->is_parallel_exec);
  }
end:
  if (mdl_backup.ticket)
  {
    /*
      We do not always immediately release transactional locks
      after ha_commit_trans() (see uses of ha_enable_transaction()),
      thus we release the commit blocker lock as soon as it's
      not needed.
    */
    thd->mdl_context.release_lock(mdl_backup.ticket);
  }
  thd->backup_commit_lock= 0;
#ifdef WITH_WSREP
  if (wsrep_is_active(thd) && is_real_trans && !error &&
      (rw_ha_count == 0 || all) &&
      wsrep_not_committed(thd))
  {
    wsrep_commit_empty(thd, all);
  }
#endif /* WITH_WSREP */

  DBUG_RETURN(error);
}
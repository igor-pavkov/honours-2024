int ha_commit_one_phase(THD *thd, bool all)
{
  THD_TRANS *trans=all ? &thd->transaction.all : &thd->transaction.stmt;
  /*
    "real" is a nick name for a transaction for which a commit will
    make persistent changes. E.g. a 'stmt' transaction inside a 'all'
    transaction is not 'real': even though it's possible to commit it,
    the changes are not durable as they might be rolled back if the
    enclosing 'all' transaction is rolled back.
    We establish the value of 'is_real_trans' by checking
    if it's an explicit COMMIT/BEGIN statement, or implicit
    commit issued by DDL (all == TRUE), or if we're running
    in autocommit mode (it's only in the autocommit mode
    ha_commit_one_phase() can be called with an empty
    transaction.all.ha_list, see why in trans_register_ha()).
  */
  bool is_real_trans= ((all || thd->transaction.all.ha_list == 0) &&
                       !(thd->variables.option_bits & OPTION_GTID_BEGIN));
  int res;
  DBUG_ENTER("ha_commit_one_phase");
  if (is_real_trans)
  {
    DEBUG_SYNC(thd, "ha_commit_one_phase");
    if ((res= thd->wait_for_prior_commit()))
      DBUG_RETURN(res);
  }
  res= commit_one_phase_2(thd, all, trans, is_real_trans);
  DBUG_RETURN(res);
}
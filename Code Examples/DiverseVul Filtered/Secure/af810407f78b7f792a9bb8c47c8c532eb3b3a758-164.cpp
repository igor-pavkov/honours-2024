static int prepare_or_error(handlerton *ht, THD *thd, bool all)
{
#ifdef WITH_WSREP
  const bool run_wsrep_hooks= wsrep_run_commit_hook(thd, all);
  if (run_wsrep_hooks && ht->flags & HTON_WSREP_REPLICATION &&
      wsrep_before_prepare(thd, all))
  {
    return(1);
  }
#endif /* WITH_WSREP */

  int err= ht->prepare(ht, thd, all);
  status_var_increment(thd->status_var.ha_prepare_count);
  if (err)
  {
      my_error(ER_ERROR_DURING_COMMIT, MYF(0), err);
  }
#ifdef WITH_WSREP
  if (run_wsrep_hooks && !err && ht->flags & HTON_WSREP_REPLICATION &&
      wsrep_after_prepare(thd, all))
  {
    err= 1;
  }
#endif /* WITH_WSREP */

  return err;
}
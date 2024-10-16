static int wsrep_after_row(THD *thd)
{
  DBUG_ENTER("wsrep_after_row");
  /* enforce wsrep_max_ws_rows */
  thd->wsrep_affected_rows++;
  if (wsrep_max_ws_rows &&
      wsrep_thd_is_local(thd) &&
      thd->wsrep_affected_rows > wsrep_max_ws_rows)
  {
    trans_rollback_stmt(thd) || trans_rollback(thd);
    my_message(ER_ERROR_DURING_COMMIT, "wsrep_max_ws_rows exceeded", MYF(0));
    DBUG_RETURN(ER_ERROR_DURING_COMMIT);
  }
  else if (wsrep_after_row_internal(thd))
  {
    DBUG_RETURN(ER_LOCK_DEADLOCK);
  }
  DBUG_RETURN(0);
}
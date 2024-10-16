int ha_abort_transaction(THD *bf_thd, THD *victim_thd, my_bool signal)
{
  DBUG_ENTER("ha_abort_transaction");
  if (!WSREP(bf_thd) &&
      !(bf_thd->variables.wsrep_OSU_method == WSREP_OSU_RSU &&
        wsrep_thd_is_toi(bf_thd))) {
    DBUG_RETURN(0);
  }

  handlerton *hton= installed_htons[DB_TYPE_INNODB];
  if (hton && hton->abort_transaction)
  {
    hton->abort_transaction(hton, bf_thd, victim_thd, signal);
  }
  else
  {
    WSREP_WARN("Cannot abort InnoDB transaction");
  }

  DBUG_RETURN(0);
}
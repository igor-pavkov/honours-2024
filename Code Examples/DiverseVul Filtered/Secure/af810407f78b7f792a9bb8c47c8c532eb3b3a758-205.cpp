int handler::ha_external_lock(THD *thd, int lock_type)
{
  int error;
  DBUG_ENTER("handler::ha_external_lock");
  /*
    Whether this is lock or unlock, this should be true, and is to verify that
    if get_auto_increment() was called (thus may have reserved intervals or
    taken a table lock), ha_release_auto_increment() was too.
  */
  DBUG_ASSERT(next_insert_id == 0);
  /* Consecutive calls for lock without unlocking in between is not allowed */
  DBUG_ASSERT(table_share->tmp_table != NO_TMP_TABLE ||
              ((lock_type != F_UNLCK && m_lock_type == F_UNLCK) ||
               lock_type == F_UNLCK));
  /* SQL HANDLER call locks/unlock while scanning (RND/INDEX). */
  DBUG_ASSERT(inited == NONE || table->open_by_handler);

  if (MYSQL_HANDLER_RDLOCK_START_ENABLED() ||
      MYSQL_HANDLER_WRLOCK_START_ENABLED() ||
      MYSQL_HANDLER_UNLOCK_START_ENABLED())
  {
    if (lock_type == F_RDLCK)
    {
      MYSQL_HANDLER_RDLOCK_START(table_share->db.str,
                                 table_share->table_name.str);
    }
    else if (lock_type == F_WRLCK)
    {
      MYSQL_HANDLER_WRLOCK_START(table_share->db.str,
                                 table_share->table_name.str);
    }
    else if (lock_type == F_UNLCK)
    {
      MYSQL_HANDLER_UNLOCK_START(table_share->db.str,
                                 table_share->table_name.str);
    }
  }

  /*
    We cache the table flags if the locking succeeded. Otherwise, we
    keep them as they were when they were fetched in ha_open().
  */
  MYSQL_TABLE_LOCK_WAIT(m_psi, PSI_TABLE_EXTERNAL_LOCK, lock_type,
    { error= external_lock(thd, lock_type); })

  DBUG_EXECUTE_IF("external_lock_failure", error= HA_ERR_GENERIC;);

  if (likely(error == 0 || lock_type == F_UNLCK))
  {
    m_lock_type= lock_type;
    cached_table_flags= table_flags();
    if (table_share->tmp_table == NO_TMP_TABLE)
      mysql_audit_external_lock(thd, table_share, lock_type);
  }

  if (MYSQL_HANDLER_RDLOCK_DONE_ENABLED() ||
      MYSQL_HANDLER_WRLOCK_DONE_ENABLED() ||
      MYSQL_HANDLER_UNLOCK_DONE_ENABLED())
  {
    if (lock_type == F_RDLCK)
    {
      MYSQL_HANDLER_RDLOCK_DONE(error);
    }
    else if (lock_type == F_WRLCK)
    {
      MYSQL_HANDLER_WRLOCK_DONE(error);
    }
    else if (lock_type == F_UNLCK)
    {
      MYSQL_HANDLER_UNLOCK_DONE(error);
    }
  }
  DBUG_RETURN(error);
}
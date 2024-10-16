int handler::ha_close(void)
{
  DBUG_ENTER("ha_close");
  /*
    Increment global statistics for temporary tables.
    In_use is 0 for tables that was closed from the table cache.
  */
  if (table->in_use)
    status_var_add(table->in_use->status_var.rows_tmp_read, rows_tmp_read);
  PSI_CALL_close_table(m_psi);
  m_psi= NULL; /* instrumentation handle, invalid after close_table() */

  /* Detach from ANALYZE tracker */
  tracker= NULL;
  
  DBUG_ASSERT(m_lock_type == F_UNLCK);
  DBUG_ASSERT(inited == NONE);
  DBUG_RETURN(close());
}
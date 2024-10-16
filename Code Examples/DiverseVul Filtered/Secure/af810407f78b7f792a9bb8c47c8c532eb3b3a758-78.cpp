void handler::rebind_psi()
{
  /*
    Notify the instrumentation that this table is now owned
    by this thread.
  */
  m_psi= PSI_CALL_rebind_table(ha_table_share_psi(), this, m_psi);
}
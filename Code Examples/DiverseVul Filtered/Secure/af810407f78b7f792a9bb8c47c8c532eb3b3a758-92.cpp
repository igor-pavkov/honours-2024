void handler::unbind_psi()
{
  /*
    Notify the instrumentation that this table is not owned
    by this thread any more.
  */
  PSI_CALL_unbind_table(m_psi);
}
  bool safely_trapped_errors()
  {
    return ((m_handled_errors > 0) && (m_unhandled_errors == 0));
  }
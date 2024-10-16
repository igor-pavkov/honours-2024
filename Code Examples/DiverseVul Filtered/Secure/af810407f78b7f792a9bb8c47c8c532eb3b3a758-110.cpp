  bool handle_condition(THD *thd,
                        uint sql_errno,
                        const char* sqlstate,
                        Sql_condition::enum_warning_level *level,
                        const char* msg,
                        Sql_condition ** cond_hdl)
  {
    *cond_hdl= NULL;
    if (sql_errno == ER_NO_SUCH_TABLE ||
        sql_errno == ER_NO_SUCH_TABLE_IN_ENGINE ||
        sql_errno == ER_WRONG_OBJECT)
    {
      m_handled_errors++;
      return TRUE;
    }

    if (*level == Sql_condition::WARN_LEVEL_ERROR)
      m_unhandled_errors++;
    return FALSE;
  }
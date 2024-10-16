  void report_error(uint code, const char *fmt, ...)
  {
    va_list args;
    va_start(args, fmt);

    if (code == 0)
      error_log_print(WARNING_LEVEL, fmt, args);
    else if (code == ER_CANNOT_LOAD_FROM_TABLE_V2)
    {
      char *db_name, *table_name;
      db_name=  va_arg(args, char *);
      table_name= va_arg(args, char *);
      my_error(code, MYF(ME_NOREFRESH), db_name, table_name);
    }
    else
      my_printv_error(code, ER(code), MYF(ME_NOREFRESH), args);

    va_end(args);
  }
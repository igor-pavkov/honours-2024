void print_keydup_error(TABLE *table, KEY *key, myf errflag)
{
  print_keydup_error(table, key,
                     ER_THD(table->in_use, ER_DUP_ENTRY_WITH_KEY_NAME),
                     errflag);
}
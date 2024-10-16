Discovered_table_list::Discovered_table_list(THD *thd_arg,
                 Dynamic_array<LEX_CSTRING*> *tables_arg,
                 const LEX_CSTRING *wild_arg) :
  thd(thd_arg), with_temps(false), tables(tables_arg)
{
  if (wild_arg->str && wild_arg->str[0])
  {
    wild= wild_arg->str;
    wend= wild + wild_arg->length;
  }
  else
    wild= 0;
}
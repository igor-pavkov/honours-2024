bool Item_in_subselect::test_limit(st_select_lex_unit *unit_arg)
{
  if (unit_arg->fake_select_lex &&
      unit_arg->fake_select_lex->test_limit())
    return(1);

  SELECT_LEX *sl= unit_arg->first_select();
  for (; sl; sl= sl->next_select())
  {
    if (sl->test_limit())
      return(1);
  }
  return(0);
}
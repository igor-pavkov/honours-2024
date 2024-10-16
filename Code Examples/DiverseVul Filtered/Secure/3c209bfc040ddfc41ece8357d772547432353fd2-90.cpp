bool Item_in_subselect::fix_having(Item *having, SELECT_LEX *select_lex)
{
  bool fix_res= 0;
  DBUG_ASSERT(thd);
  if (!having->fixed)
  {
    select_lex->having_fix_field= 1;
    fix_res= having->fix_fields(thd, 0);
    select_lex->having_fix_field= 0;
  }
  return fix_res;
}
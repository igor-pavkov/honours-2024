bool Item_subselect::mark_as_dependent(THD *thd, st_select_lex *select, 
                                       Item *item)
{
  if (inside_first_fix_fields)
  {
    is_correlated= TRUE;
    Ref_to_outside *upper;
    if (!(upper= new (thd->stmt_arena->mem_root) Ref_to_outside()))
      return TRUE;
    upper->select= select;
    upper->item= item;
    if (upper_refs.push_back(upper, thd->stmt_arena->mem_root))
      return TRUE;
  }
  return FALSE;
}
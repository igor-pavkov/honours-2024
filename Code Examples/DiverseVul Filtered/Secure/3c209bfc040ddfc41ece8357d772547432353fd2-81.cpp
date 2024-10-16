void Item_subselect::fix_after_pullout(st_select_lex *new_parent,
                                       Item **ref, bool merge)
{
  recalc_used_tables(new_parent, TRUE);
  parent_select= new_parent;
}
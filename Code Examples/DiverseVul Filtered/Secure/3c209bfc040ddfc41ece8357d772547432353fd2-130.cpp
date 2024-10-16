void Item_in_subselect::fix_after_pullout(st_select_lex *new_parent,
                                          Item **ref, bool merge)
{
  left_expr->fix_after_pullout(new_parent, &left_expr, merge);
  Item_subselect::fix_after_pullout(new_parent, ref, merge);
  used_tables_cache |= left_expr->used_tables();
}
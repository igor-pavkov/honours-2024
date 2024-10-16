void Item_in_subselect::update_used_tables()
{
  Item_subselect::update_used_tables();
  left_expr->update_used_tables();
  //used_tables_cache |= left_expr->used_tables();
  used_tables_cache= Item_subselect::used_tables() | left_expr->used_tables();
}
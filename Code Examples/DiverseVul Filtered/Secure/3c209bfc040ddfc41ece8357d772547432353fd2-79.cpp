inline bool Item_in_subselect::left_expr_has_null()
{
  return (*(optimizer->get_cache()))->null_value_inside;
}
void Item_maxmin_subselect::no_rows_in_result()
{
  /*
    Subquery predicates outside of the SELECT list must be evaluated in order
    to possibly filter the special result row generated for implicit grouping
    if the subquery is in the HAVING clause.
    If the predicate is constant, we need its actual value in the only result
    row for queries with implicit grouping.
  */
  if (parsing_place != SELECT_LIST || const_item())
    return;
  value= Item_cache::get_cache(thd, new (thd->mem_root) Item_null(thd));
  null_value= 0;
  was_values= 0;
  make_const();
}
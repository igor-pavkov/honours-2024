void Item_allany_subselect::no_rows_in_result()
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
  value= 0;
  null_value= 0;
  was_null= 0;
  make_const();
}
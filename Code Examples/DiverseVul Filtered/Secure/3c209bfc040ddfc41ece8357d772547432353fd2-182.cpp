Item *Item_subselect::get_tmp_table_item(THD *thd_arg)
{
  if (!with_sum_func && !const_item())
    return new (thd->mem_root) Item_temptable_field(thd_arg, result_field);
  return copy_or_same(thd_arg);
}
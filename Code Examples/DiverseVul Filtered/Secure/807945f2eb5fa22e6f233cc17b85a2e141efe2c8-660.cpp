Item *Item_func_le::negated_item(THD *thd) /* a <= b  ->  a > b */
{
  return new (thd->mem_root) Item_func_gt(thd, args[0], args[1]);
}
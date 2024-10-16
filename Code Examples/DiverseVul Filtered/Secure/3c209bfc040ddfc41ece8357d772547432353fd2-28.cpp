void Item_subselect::update_used_tables()
{
  if (!forced_const)
  {
    recalc_used_tables(parent_select, FALSE);
    if (!(engine->uncacheable() & ~UNCACHEABLE_EXPLAIN))
    {
      // did all used tables become static?
      if (!(used_tables_cache & ~engine->upper_select_const_tables()) &&
          ! with_recursive_reference)
        const_item_cache= 1;
    }
  }
}
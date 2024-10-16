bool Item_in_subselect::expr_cache_is_needed(THD *thd)
{
  return (optimizer_flag(thd, OPTIMIZER_SWITCH_SUBQUERY_CACHE) &&
          !(engine->uncacheable() & (UNCACHEABLE_RAND |
                                     UNCACHEABLE_SIDEEFFECT)) &&
          !with_recursive_reference);
}
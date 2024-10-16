bool Item_subselect::expr_cache_is_needed(THD *thd)
{
  return ((engine->uncacheable() & UNCACHEABLE_DEPENDENT) &&
          engine->cols() == 1 &&
          optimizer_flag(thd, OPTIMIZER_SWITCH_SUBQUERY_CACHE) &&
          !(engine->uncacheable() & (UNCACHEABLE_RAND |
                                     UNCACHEABLE_SIDEEFFECT)) &&
          !with_recursive_reference);
}
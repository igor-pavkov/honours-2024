bool Item_in_subselect::exec()
{
  DBUG_ENTER("Item_in_subselect::exec");
  DBUG_ASSERT(fixed);
  /*
    Initialize the cache of the left predicate operand. This has to be done as
    late as now, because Cached_item directly contains a resolved field (not
    an item, and in some cases (when temp tables are created), these fields
    end up pointing to the wrong field. One solution is to change Cached_item
    to not resolve its field upon creation, but to resolve it dynamically
    from a given Item_ref object.
    TODO: the cache should be applied conditionally based on:
    - rules - e.g. only if the left operand is known to be ordered, and/or
    - on a cost-based basis, that takes into account the cost of a cache
      lookup, the cache hit rate, and the savings per cache hit.
  */
  if (!left_expr_cache && (test_strategy(SUBS_MATERIALIZATION)))
    init_left_expr_cache();

  /*
    If the new left operand is already in the cache, reuse the old result.
    Use the cached result only if this is not the first execution of IN
    because the cache is not valid for the first execution.
  */
  if (!first_execution && left_expr_cache &&
      test_if_item_cache_changed(*left_expr_cache) < 0)
    DBUG_RETURN(FALSE);

  /*
    The exec() method below updates item::value, and item::null_value, thus if
    we don't call it, the next call to item::val_int() will return whatever
    result was computed by its previous call.
  */
  DBUG_RETURN(Item_subselect::exec());
}
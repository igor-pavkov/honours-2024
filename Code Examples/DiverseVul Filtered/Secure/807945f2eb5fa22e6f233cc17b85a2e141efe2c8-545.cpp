  Item_cache(THD *thd, const Type_handler *handler):
    Item(thd),
    Type_handler_hybrid_field_type(handler),
    example(0), cached_field(0),
    value_cached(0),
    used_table_map(0)
  {
    maybe_null= 1;
    null_value= 1;
    null_value_inside= true;
  }
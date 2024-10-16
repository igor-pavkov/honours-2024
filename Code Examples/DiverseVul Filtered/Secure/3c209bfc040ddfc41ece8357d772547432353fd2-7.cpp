table_map Item_subselect::used_tables() const
{
  return (table_map) ((engine->uncacheable() & ~UNCACHEABLE_EXPLAIN)? 
                      used_tables_cache : 0L);
}
uint subselect_union_engine::cols()
{
  DBUG_ASSERT(unit->is_prepared());  // should be called after fix_fields()
  return unit->types.elements;
}
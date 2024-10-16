int subselect_union_engine::prepare(THD *thd_arg)
{
  set_thd(thd_arg);
  return unit->prepare(thd, result, SELECT_NO_UNLOCK);
}
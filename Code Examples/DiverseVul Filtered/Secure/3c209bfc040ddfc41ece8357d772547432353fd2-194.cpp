int subselect_hash_sj_engine::prepare(THD *thd_arg)
{
  /*
    Create and optimize the JOIN that will be used to materialize
    the subquery if not yet created.
  */
  set_thd(thd_arg);
  return materialize_engine->prepare(thd);
}
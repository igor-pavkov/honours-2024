bool subselect_hash_sj_engine::change_result(Item_subselect *si,
                                             select_result_interceptor *res,
                                             bool temp __attribute__((unused)))
{
  DBUG_ASSERT(FALSE);
  return TRUE;
}
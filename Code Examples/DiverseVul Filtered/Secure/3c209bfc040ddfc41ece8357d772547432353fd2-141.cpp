subselect_hash_sj_engine::~subselect_hash_sj_engine()
{
  delete lookup_engine;
  delete result;
  if (tmp_table)
    free_tmp_table(thd, tmp_table);
}
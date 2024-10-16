int subselect_union_engine::exec()
{
  char const *save_where= thd->where;
  int res= unit->exec();
  thd->where= save_where;
  return res;
}
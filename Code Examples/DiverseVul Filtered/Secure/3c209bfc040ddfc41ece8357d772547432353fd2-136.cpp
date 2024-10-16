enum Item_result Item_singlerow_subselect::result_type() const
{
  return engine->type();
}
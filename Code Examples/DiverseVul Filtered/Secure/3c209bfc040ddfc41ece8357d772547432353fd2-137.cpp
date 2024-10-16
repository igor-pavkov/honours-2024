void Item_singlerow_subselect::reset()
{
  Item_subselect::reset();
  if (value)
  {
    for(uint i= 0; i < engine->cols(); i++)
      row[i]->set_null();
  }
}
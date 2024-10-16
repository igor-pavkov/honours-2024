void Item_singlerow_subselect::bring_value()
{
  if (!exec() && assigned())
  {
    null_value= true;
    for (uint i= 0; i < max_columns ; i++)
    {
      if (!row[i]->null_value)
      {
        null_value= false;
        return;
      }
    }
  }
  else
    reset();
}
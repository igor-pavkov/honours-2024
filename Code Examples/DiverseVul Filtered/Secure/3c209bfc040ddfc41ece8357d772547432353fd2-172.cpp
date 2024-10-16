bool Item_subselect::fix_length_and_dec()
{
  if (engine->fix_length_and_dec(0))
    return TRUE;
  return FALSE;
}
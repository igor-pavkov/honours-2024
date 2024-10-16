bool Item_subselect::mark_as_eliminated_processor(void *arg)
{
  eliminated= TRUE;
  return FALSE;
}
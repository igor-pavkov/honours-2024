bool Item_subselect::eliminate_subselect_processor(void *arg)
{
  unit->item= NULL;
  unit->exclude();
  eliminated= TRUE;
  return FALSE;
}
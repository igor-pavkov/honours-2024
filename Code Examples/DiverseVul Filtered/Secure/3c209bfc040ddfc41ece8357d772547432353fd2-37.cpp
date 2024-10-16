bool Item_exists_subselect::fix_fields(THD *thd, Item **ref)
{
  DBUG_ENTER("Item_exists_subselect::fix_fields");
  if (exists_transformed)
    DBUG_RETURN( !( (*ref)= new (thd->mem_root) Item_int(thd, 1)));
  DBUG_RETURN(Item_subselect::fix_fields(thd, ref));
}
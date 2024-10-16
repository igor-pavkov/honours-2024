Item_subselect::select_transformer(JOIN *join)
{
  DBUG_ENTER("Item_subselect::select_transformer");
  DBUG_ASSERT(thd == join->thd);
  DBUG_RETURN(false);
}
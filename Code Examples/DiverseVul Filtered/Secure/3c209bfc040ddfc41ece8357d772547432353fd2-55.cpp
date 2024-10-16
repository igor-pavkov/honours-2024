Item_allany_subselect::select_transformer(JOIN *join)
{
  DBUG_ENTER("Item_allany_subselect::select_transformer");
  DBUG_ASSERT((in_strategy & ~(SUBS_MAXMIN_INJECTED | SUBS_MAXMIN_ENGINE |
                               SUBS_IN_TO_EXISTS | SUBS_STRATEGY_CHOSEN)) == 0);
  if (upper_item)
    upper_item->show= 1;
  DBUG_RETURN(select_in_like_transformer(join));
}
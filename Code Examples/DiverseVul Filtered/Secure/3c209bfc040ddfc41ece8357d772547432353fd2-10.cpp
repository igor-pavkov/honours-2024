bool Item_subselect::const_item() const
{
  DBUG_ASSERT(thd);
  return (thd->lex->context_analysis_only || with_recursive_reference ?
          FALSE :
          forced_const || const_item_cache);
}
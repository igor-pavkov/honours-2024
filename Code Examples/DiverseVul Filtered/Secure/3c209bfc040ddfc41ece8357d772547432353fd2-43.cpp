void Item_subselect::init_expr_cache_tracker(THD *thd)
{
  if(!expr_cache)
    return;

  Explain_query *qw= thd->lex->explain;
  DBUG_ASSERT(qw);
  Explain_node *node= qw->get_node(unit->first_select()->select_number);
  if (!node)
    return;
  DBUG_ASSERT(expr_cache->type() == Item::EXPR_CACHE_ITEM);
  node->cache_tracker= ((Item_cache_wrapper *)expr_cache)->init_tracker(qw->mem_root);
}
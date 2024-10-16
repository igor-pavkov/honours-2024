subselect_hash_sj_engine::make_unique_engine()
{
  Item_in_subselect *item_in= (Item_in_subselect *) item;
  Item_iterator_row it(item_in->left_expr);
  /* The only index on the temporary table. */
  KEY *tmp_key= tmp_table->key_info;
  JOIN_TAB *tab;

  DBUG_ENTER("subselect_hash_sj_engine::make_unique_engine");

  /*
    Create and initialize the JOIN_TAB that represents an index lookup
    plan operator into the materialized subquery result. Notice that:
    - this JOIN_TAB has no corresponding JOIN (and doesn't need one), and
    - here we initialize only those members that are used by
      subselect_uniquesubquery_engine, so these objects are incomplete.
  */
  if (!(tab= (JOIN_TAB*) thd->alloc(sizeof(JOIN_TAB))))
    DBUG_RETURN(NULL);

  tab->table= tmp_table;
  tab->preread_init_done= FALSE;
  tab->ref.tmp_table_index_lookup_init(thd, tmp_key, it, FALSE);

  DBUG_RETURN(new subselect_uniquesubquery_engine(thd, tab, item,
                                                  semi_join_conds));
}
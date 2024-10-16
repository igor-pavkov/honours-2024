bool subselect_hash_sj_engine::make_semi_join_conds()
{
  /*
    Table reference for tmp_table that is used to resolve column references
    (Item_fields) to columns in tmp_table.
  */
  TABLE_LIST *tmp_table_ref;
  /* Name resolution context for all tmp_table columns created below. */
  Name_resolution_context *context;
  Item_in_subselect *item_in= (Item_in_subselect *) item;

  DBUG_ENTER("subselect_hash_sj_engine::make_semi_join_conds");
  DBUG_ASSERT(semi_join_conds == NULL);

  if (!(semi_join_conds= new (thd->mem_root) Item_cond_and(thd)))
    DBUG_RETURN(TRUE);

  if (!(tmp_table_ref= (TABLE_LIST*) thd->alloc(sizeof(TABLE_LIST))))
    DBUG_RETURN(TRUE);

  tmp_table_ref->init_one_table(STRING_WITH_LEN(""),
                                tmp_table->alias.c_ptr(),
                                tmp_table->alias.length(),
                                NULL, TL_READ);
  tmp_table_ref->table= tmp_table;

  context= new Name_resolution_context;
  context->init();
  context->first_name_resolution_table=
    context->last_name_resolution_table= tmp_table_ref;
  semi_join_conds_context= context;
  
  for (uint i= 0; i < item_in->left_expr->cols(); i++)
  {
    /* New equi-join condition for the current column. */
    Item_func_eq *eq_cond;
    /* Item for the corresponding field from the materialized temp table. */
    Item_field *right_col_item;

    if (!(right_col_item= new (thd->mem_root)
          Item_temptable_field(thd, context, tmp_table->field[i])) ||
        !(eq_cond= new (thd->mem_root)
          Item_func_eq(thd, item_in->left_expr->element_index(i),
                       right_col_item)) ||
        (((Item_cond_and*)semi_join_conds)->add(eq_cond, thd->mem_root)))
    {
      delete semi_join_conds;
      semi_join_conds= NULL;
      DBUG_RETURN(TRUE);
    }
  }
  if (semi_join_conds->fix_fields(thd, (Item**)&semi_join_conds))
    DBUG_RETURN(TRUE);

  DBUG_RETURN(FALSE);
}
static bool check_equality_for_exist2in(Item_func *func,
                                        bool allow_subselect,
                                        Item_ident **local_field,
                                        Item **outer_exp)
{
  Item **args;
  if (func->functype() != Item_func::EQ_FUNC)
    return FALSE;
  DBUG_ASSERT(func->argument_count() == 2);
  args= func->arguments();
  if (args[0]->real_type() == Item::FIELD_ITEM &&
      args[0]->all_used_tables() != OUTER_REF_TABLE_BIT &&
      args[1]->all_used_tables() == OUTER_REF_TABLE_BIT &&
      (allow_subselect || !args[1]->has_subquery()))
  {
    /* It is Item_field or Item_direct_view_ref) */
    DBUG_ASSERT(args[0]->type() == Item::FIELD_ITEM ||
                args[0]->type() == Item::REF_ITEM);
    *local_field= (Item_ident *)args[0];
    *outer_exp= args[1];
    return TRUE;
  }
  else if (args[1]->real_type() == Item::FIELD_ITEM &&
           args[1]->all_used_tables() != OUTER_REF_TABLE_BIT &&
           args[0]->all_used_tables() == OUTER_REF_TABLE_BIT &&
           (allow_subselect || !args[0]->has_subquery()))
  {
    /* It is Item_field or Item_direct_view_ref) */
    DBUG_ASSERT(args[1]->type() == Item::FIELD_ITEM ||
                args[1]->type() == Item::REF_ITEM);
    *local_field= (Item_ident *)args[1];
    *outer_exp= args[0];
    return TRUE;
  }

  return FALSE;
}
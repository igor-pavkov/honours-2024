bool Item_exists_subselect::select_prepare_to_be_in()
{
  bool trans_res= FALSE;
  DBUG_ENTER("Item_exists_subselect::select_prepare_to_be_in");
  if (!optimizer &&
      thd->lex->sql_command == SQLCOM_SELECT &&
      !unit->first_select()->is_part_of_union() &&
      optimizer_flag(thd, OPTIMIZER_SWITCH_EXISTS_TO_IN) &&
      (is_top_level_item() ||
       (upper_not && upper_not->is_top_level_item())))
  {
    Query_arena *arena, backup;
    bool result;
    arena= thd->activate_stmt_arena_if_needed(&backup);
    result= (!(optimizer= new (thd->mem_root) Item_in_optimizer(thd, new (thd->mem_root) Item_int(thd, 1), this)));
    if (arena)
      thd->restore_active_arena(arena, &backup);
    if (result)
      trans_res= TRUE;
    else
      substitution= optimizer;
  }
  DBUG_RETURN(trans_res);
}
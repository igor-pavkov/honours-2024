bool Item_in_subselect::setup_mat_engine()
{
  subselect_hash_sj_engine       *mat_engine= NULL;
  subselect_single_select_engine *select_engine;

  DBUG_ENTER("Item_in_subselect::setup_mat_engine");
  DBUG_ASSERT(thd);

  /*
    The select_engine (that executes transformed IN=>EXISTS subselects) is
    pre-created at parse time, and is stored in statement memory (preserved
    across PS executions).
  */
  DBUG_ASSERT(engine->engine_type() == subselect_engine::SINGLE_SELECT_ENGINE);
  select_engine= (subselect_single_select_engine*) engine;

  /* Create/initialize execution objects. */
  if (!(mat_engine= new subselect_hash_sj_engine(thd, this, select_engine)))
    DBUG_RETURN(TRUE);

  if (mat_engine->prepare(thd) ||
      mat_engine->init(&select_engine->join->fields_list,
                       engine->get_identifier()))
    DBUG_RETURN(TRUE);

  engine= mat_engine;
  DBUG_RETURN(FALSE);
}
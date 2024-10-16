bool subselect_uniquesubquery_engine::copy_ref_key(bool skip_constants)
{
  DBUG_ENTER("subselect_uniquesubquery_engine::copy_ref_key");

  for (store_key **copy= tab->ref.key_copy ; *copy ; copy++)
  {
    enum store_key::store_key_result store_res;
    if (skip_constants && (*copy)->store_key_is_const())
      continue;
    store_res= (*copy)->copy();
    tab->ref.key_err= store_res;

    if (store_res == store_key::STORE_KEY_FATAL)
    {
      /*
       Error converting the left IN operand to the column type of the right
       IN operand. 
      */
      DBUG_RETURN(true);
    }
  }
  DBUG_RETURN(false);
}
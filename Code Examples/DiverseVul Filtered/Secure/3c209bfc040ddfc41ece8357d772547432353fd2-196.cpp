bool Item_in_subselect::init_cond_guards()
{
  DBUG_ASSERT(thd);
  uint cols_num= left_expr->cols();
  if (!abort_on_null && !pushed_cond_guards &&
      (left_expr->maybe_null || cols_num > 1))
  {
    if (!(pushed_cond_guards= (bool*)thd->alloc(sizeof(bool) * cols_num)))
        return TRUE;
    for (uint i= 0; i < cols_num; i++)
      pushed_cond_guards[i]= TRUE;
  }
  return FALSE;
}
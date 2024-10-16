void Ordered_key::cleanup()
{
  /*
    Currently these keys are recreated for each PS re-execution, thus
    there is nothing to cleanup, the whole object goes away after execution
    is over. All handler related initialization/deinitialization is done by
    the parent subselect_rowid_merge_engine object.
  */
}
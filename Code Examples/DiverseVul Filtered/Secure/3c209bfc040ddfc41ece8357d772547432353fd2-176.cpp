bool Item_in_subselect::fix_length_and_dec()
{
  DBUG_ENTER("Item_in_subselect::fix_length_and_dec");
  init_length_and_dec();
  /*
    Unlike Item_exists_subselect, LIMIT 1 is set later for
    Item_in_subselect, depending on the chosen strategy.
  */
  DBUG_RETURN(FALSE);
}
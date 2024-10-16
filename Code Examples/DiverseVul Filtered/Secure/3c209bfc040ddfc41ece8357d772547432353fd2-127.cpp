bool subselect_union_engine::no_rows()
{
  /* Check if we got any rows when reading UNION result from temp. table: */
  if (unit->fake_select_lex)
  {
    JOIN *join= unit->fake_select_lex->join;
    if (join)
      return MY_TEST(!join->send_records);
    return false;
  }
  return MY_TEST(!(((select_union_direct *)(unit->get_union_result()))
                                                            ->send_records));
}
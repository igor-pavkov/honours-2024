void Item_subselect::print(String *str, enum_query_type query_type)
{
  if (query_type & QT_ITEM_SUBSELECT_ID_ONLY)
  {
    str->append("(subquery#");
    if (unit && unit->first_select())
    {
      char buf[64];
      ll2str(unit->first_select()->select_number, buf, 10, 0); 
      str->append(buf);
    }
    else
      str->append("NULL"); // TODO: what exactly does this mean?

    str->append(")");
    return;
  }
  if (engine)
  {
    str->append('(');
    engine->print(str, query_type);
    str->append(')');
  }
  else
    str->append("(...)");
}
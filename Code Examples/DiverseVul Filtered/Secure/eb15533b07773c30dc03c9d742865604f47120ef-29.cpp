  Expression_Obj Parser::lex_interp_string()
  {
    Expression_Obj rv;
    if ((rv = lex_interp< re_string_double_open, re_string_double_close >())) return rv;
    if ((rv = lex_interp< re_string_single_open, re_string_single_close >())) return rv;
    return rv;
  }
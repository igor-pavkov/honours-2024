  Selector_List_Obj Parser::parse_selector(const char* beg, Context& ctx, Backtraces traces, ParserState pstate, const char* source)
  {
    Parser p = Parser::from_c_str(beg, ctx, traces, pstate, source);
    // ToDo: ruby sass errors on parent references
    // ToDo: remap the source-map entries somehow
    return p.parse_selector_list(false);
  }
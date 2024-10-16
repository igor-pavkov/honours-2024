int puma_parser_init(puma_parser *parser)  {
  int cs = 0;
  
#line 53 "ext/puma_http11/http11_parser.c"
	{
	cs = puma_parser_start;
	}

#line 89 "ext/puma_http11/http11_parser.rl"
  parser->cs = cs;
  parser->body_start = 0;
  parser->content_len = 0;
  parser->mark = 0;
  parser->nread = 0;
  parser->field_len = 0;
  parser->field_start = 0;
  parser->request = Qnil;
  parser->body = Qnil;

  return 1;
}
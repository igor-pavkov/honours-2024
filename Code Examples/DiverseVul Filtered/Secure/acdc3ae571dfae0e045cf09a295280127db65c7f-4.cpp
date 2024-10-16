int puma_parser_finish(puma_parser *parser)
{
  if (puma_parser_has_error(parser) ) {
    return -1;
  } else if (puma_parser_is_finished(parser) ) {
    return 1;
  } else {
    return 0;
  }
}
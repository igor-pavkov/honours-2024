int puma_parser_has_error(puma_parser *parser) {
  return parser->cs == puma_parser_error;
}
int puma_parser_is_finished(puma_parser *parser) {
  return parser->cs >= puma_parser_first_final;
}
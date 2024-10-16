char *filter_printf(const char *filter, const char *user) {
  char *result = malloc(filter_result_len(filter, user, NULL));
  filter_result_len(filter, user, result);
  return result;
}
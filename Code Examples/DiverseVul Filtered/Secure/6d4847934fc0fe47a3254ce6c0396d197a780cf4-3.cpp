char *rfc1123(char *buf, time_t t)
{
  strftime(buf, 64, "%a, %d %b %Y %T GMT", gmtime(&t));

  return buf;
}
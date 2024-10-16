void error_time(int stat, char *str)
{
  header_time(stat, str, 0);
  xprintf("<html><head><title>%d %s</title></head>"
    "<body><h3>%d %s</h3></body></html>", stat, str, stat, str);
}
void header_time(int stat, char *str, char *more)
{
  char buf[64];

  xprintf("HTTP/1.1 %d %s\r\nServer: toybox httpd/%s\r\nDate: %s\r\n%s"
    "Connection: close\r\n\r\n", stat, str, TOYBOX_VERSION,
    rfc1123(buf, time(0)), more ? : "");
}
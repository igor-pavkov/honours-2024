char *mime(char *file)
{
  char *s = strrchr(file, '.'), *types[] = {
    "asc\0text/plain", "bin\0application/octet-stream", "bmp\0image/bmp",
    "cpio\0application/x-cpio", "css\0text/css", "doc\0application/msword",
    "dtd\0text/xml", "dvi\0application/x-dvi", "gif\0image/gif",
    "htm\0text/html", "html\0text/html", "jar\0applicat/x-java-archive",
    "jpeg\0image/jpeg", "jpg\0image/jpeg", "js\0application/x-javascript",
    "mp3\0audio/mpeg", "mp4\0video/mp4", "mpg\0video/mpeg",
    "ogg\0application/ogg", "pbm\0image/x-portable-bitmap",
    "pdf\0application/pdf", "png\0image/png",
    "ppt\0application/vnd.ms-powerpoint", "ps\0application/postscript",
    "rtf\0text/rtf", "sgml\0text/sgml", "svg\0image/svg+xml",
    "tar\0application/x-tar", "tex\0application/x-tex", "tiff\0image/tiff",
    "txt\0text/plain", "wav\0audio/x-wav", "xls\0application/vnd.ms-excel",
    "xml\0tet/xml", "zip\0application/zip"
  };
  int i;

  strcpy(toybuf, "text/plain");
  if (s++) for (i = 0; i<ARRAY_LEN(types); i++) {
    if (strcasecmp(s, types[i])) continue;
    strcpy(toybuf, types[i]+strlen(types[i])+1);
    break;
  }
  if (!strncmp(toybuf, "text/", 5)) strcat(toybuf, "; charset=UTF-8");

  return toybuf;
}
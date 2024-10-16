static int fetch_tempfile(char *line, void *data)
{
  FILE *fp = data;

  if (!line)
    rewind(fp);
  else if (fputs(line, fp) == EOF || fputc('\n', fp) == EOF)
    return -1;
  return 0;
}
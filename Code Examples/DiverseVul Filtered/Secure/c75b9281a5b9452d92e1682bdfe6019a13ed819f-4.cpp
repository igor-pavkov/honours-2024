static void copy_related (const char *inName, const char *outName,
			  const char *old_ext, const char *new_ext)
{
  size_t name_len = strlen(inName);
  const size_t old_len  = strlen(old_ext);
  const size_t new_len  = strlen(new_ext);

  char *in = malloc(name_len - old_len + new_len + 1);
  strncpy(in, inName, (name_len - old_len));
  strcpy(&in[(name_len - old_len)], new_ext);
  FILE *inFile = fopen(in, "rb");
  if (!inFile) {
    free(in);
    return;
  }
  name_len = strlen(outName);
  char *out = malloc(name_len - old_len + new_len + 1);
  if (!out) {
    fprintf(stderr, "%s:%d: couldn't copy related file!\n",
	    __FILE__, __LINE__);
    fclose(inFile);
    free(in);
    free(out);
    return;
  }
  strncpy(out, outName, (name_len - old_len));
  strcpy(&out[(name_len - old_len)], new_ext);

  FILE *outFile = fopen(out, "wb");

  int c;
  while ((c = fgetc(inFile)) != EOF) {
    fputc(c, outFile);
  }
  fclose(inFile);
  fclose(outFile);
  free(in);
  free(out);
}
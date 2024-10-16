int cli_scansis(int desc, cli_ctx *ctx) {
  FILE *f;
  int i;
  char *tmpd;
  uint32_t uid[4];

  cli_dbgmsg("in scansis()\n");

  if (!(tmpd = cli_gentemp(ctx->engine->tmpdir)))    
    return CL_ETMPDIR;
  if (mkdir(tmpd, 0700)) {
    cli_dbgmsg("SIS: Can't create temporary directory %s\n", tmpd);
    free(tmpd);
    return CL_ETMPDIR;
  }
  if (ctx->engine->keeptmp)
    cli_dbgmsg("SIS: Extracting files to %s\n", tmpd);

  if ((i=dup(desc))==-1) {
    cli_dbgmsg("SIS: dup() failed\n");
    cli_rmdirs(tmpd);
    free(tmpd);
    return CL_EDUP;
  }
  if (!(f=fdopen(i, "rb"))) {
    cli_dbgmsg("SIS: fdopen() failed\n");
    close(i);
    cli_rmdirs(tmpd);
    free(tmpd);
    return CL_EOPEN;
  }
  rewind(f);
  if (fread(uid, 16, 1, f)!=1) {
    cli_dbgmsg("SIS: unable to read UIDs\n");
    cli_rmdirs(tmpd);
    free(tmpd);
    fclose(f);
    return CL_EREAD;
  }

  cli_dbgmsg("SIS: UIDS %x %x %x - %x\n", EC32(uid[0]), EC32(uid[1]), EC32(uid[2]), EC32(uid[3]));
  if (uid[2]==le32_to_host(0x10000419)) {
    i=real_scansis(f, ctx, tmpd);
  }
  else if(uid[0]==le32_to_host(0x10201a7a)) {
    i=real_scansis9x(f, ctx, tmpd);
  }
  else {
    cli_dbgmsg("SIS: UIDs failed to match\n");
    i=CL_EFORMAT;
  }

  if (!ctx->engine->keeptmp)
    cli_rmdirs(tmpd);

  free(tmpd);
  fclose(f);
  return i;
}
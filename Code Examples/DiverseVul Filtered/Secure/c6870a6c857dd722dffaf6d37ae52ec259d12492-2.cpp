static int real_scansis9x(FILE *f, cli_ctx *ctx, const char *tmpd) {
  struct SISTREAM stream;
  struct SISTREAM *s = &stream;
  uint32_t field, optst[]={T_CONTROLLERCHECKSUM, T_DATACHECKSUM, T_COMPRESSED};
  unsigned int i;

  s->f = f;
  s->smax = 0;
  s->sleft = 0;
  s->level = 0;

  if (getfield(s, &field) || field!=T_CONTENTS)
    return CL_CLEAN;
  s->level++;

  for (i=0;i<3;) {
    if (getfield(s, &field)) return CL_CLEAN;
    for (;i<3;i++) {
      if (field==optst[i]) {
	if (skipthis(s)) return CL_CLEAN;
	i++;
	break;
      }
    }
  }
  if (field!=T_COMPRESSED) return CL_CLEAN;

  i=0;
  while (1) { /* 1DATA */
    if (getfield(s, &field) || field!=T_DATA) break;

    s->level++; 
    while(1) { /* DATA::ARRAY */
      uint32_t atype;
      if (getfield(s, &field) || field!=T_ARRAY || getd(s, &atype) || atype!=T_DATAUNIT || s->fsize[s->level]<4) break;
      s->fsize[s->level]-=4;

      s->level++;
      while (s->fsize[s->level-1] && !getsize(s)) { /* FOREACH DATA::ARRAY::DATAUNITs */
	cli_dbgmsg("SIS: %d:Got dataunit element with size %x\n", s->level, s->fsize[s->level]);
	if (ALIGN4(s->fsize[s->level]) < s->fsize[s->level-1]) 
	  s->fsize[s->level-1]-=ALIGN4(s->fsize[s->level]);
	else
	  s->fsize[s->level-1]=0;

	s->level++;
	while(1) { /* DATA::ARRAY::DATAUNIT[x]::ARRAY */
	  if(getfield(s, &field) || field!=T_ARRAY || getd(s, &atype) || atype!=T_FILEDATA || s->fsize[s->level]<4) break;
	  s->fsize[s->level]-=4;

	  s->level++;
	  while (s->fsize[s->level-1] && !getsize(s)) { /* FOREACH DATA::ARRAY::DATAUNIT[x]::ARRAY::FILEDATA */
	    uint32_t usize, usizeh;
	    void *src, *dst;
	    char tempf[1024];
	    uLongf uusize;
	    int fd;

	    cli_dbgmsg("SIS: %d:Got filedata element with size %x\n", s->level, s->fsize[s->level]);
	    if (ALIGN4(s->fsize[s->level]) < s->fsize[s->level-1]) 
	      s->fsize[s->level-1]-=ALIGN4(s->fsize[s->level]);
	    else
	      s->fsize[s->level-1]=0;

	    s->level++;
	    while(1) { /* DATA::ARRAY::DATAUNIT[x]::ARRAY::FILEDATA[x]::COMPRESSED */
	      if(getfield(s, &field) || field!=T_COMPRESSED || getd(s, &field) || getd(s, &usize) || getd(s, &usizeh) || usizeh) break;
	      s->fsize[s->level]-=12;
	      cli_dbgmsg("SIS: File is%s compressed - size %x -> %x\n", (field)?"":" not", s->fsize[s->level], usize);
	      snprintf(tempf, 1024, "%s"PATHSEP"sis9x%02d", tmpd, i++);
	      tempf[1023]='\0';
	      fseek(s->f, -(long)s->sleft, SEEK_CUR);
	      s->sleft = s->smax = 0;

	      if (cli_checklimits("sis", ctx,ALIGN4(s->fsize[s->level]), 0, 0)!=CL_CLEAN) break;
	      if (!(src=cli_malloc(ALIGN4(s->fsize[s->level])))) break;
	      if (fread(src, ALIGN4(s->fsize[s->level]),1,s->f) != 1) {
		free(src);
		break;
	      }

	      if(field) { /* compressed */
		int zresult;

		if (usize<=s->fsize[s->level]*3 && cli_checklimits("sis", ctx, s->fsize[s->level]*3, 0, 0)==CL_CLEAN)
		  uusize=s->fsize[s->level]*3;
		else if (cli_checklimits("sis", ctx, usize, 0, 0)==CL_CLEAN)
		  uusize=usize;
		else {
		  free(src);
		  break;
		}

		if (!(dst=cli_malloc(uusize))) {
		  free(src);
		  break;
		}
		zresult=uncompress(dst, &uusize, src, s->fsize[s->level]);
		free(src);
		if (zresult!=Z_OK) {
		  cli_dbgmsg("SIS: Inflate failure (%d)\n", zresult);
		  free(dst);
		  break;
		}
		if ((uLongf)usize != uusize)
		  cli_dbgmsg("SIS: Warning: expected size %lx but got %lx\n", (uLongf)usize, uusize);
		else
		  cli_dbgmsg("SIS: File successfully inflated\n");
	      } else { /* not compressed */
		dst = src;
		uusize = s->fsize[s->level];
	      }
	      if ((fd=open(tempf, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0600))==-1) {
		cli_errmsg("SIS: unable to create output file %s - aborting.", tempf);
		free(dst);
		break;
	      }
	      if (cli_writen(fd, dst, uusize)!=(int)uusize) {
		free(dst);
		close(fd);
		break;
	      }
	      free(dst);
	      if (cli_magic_scandesc(fd, ctx) == CL_VIRUS) {
		close(fd);
		return CL_VIRUS;
	      }
	      close(fd);
	      break;
	    } /* DATA::ARRAY::DATAUNIT[x]::ARRAY::FILEDATA[x]::COMPRESSED */
	    s->level--;
	    seeknext(s);
	  } /* FOREACH DATA::ARRAY::DATAUNIT[x]::ARRAY::FILEDATAs */
	  s->level--;
	  break;
	} /* DATA::ARRAY::DATAUNIT[x]::ARRAY */
	s->level--;
	seeknext(s);
      } /* FOREACH DATA::ARRAY::DATAUNITs */
      s->level--;
      break;
    } /* DATA::ARRAY */
    s->level--;
    seeknext(s);
  }
  return CL_CLEAN;
}
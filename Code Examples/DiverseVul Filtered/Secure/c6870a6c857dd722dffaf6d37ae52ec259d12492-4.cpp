static int real_scansis(FILE *f, cli_ctx *ctx, const char *tmpd) {
  struct {
    uint16_t filesum;
    uint16_t langs;
    uint16_t files;
    uint16_t deps;
    uint16_t ulangs;
    uint16_t instfiles;
    uint16_t drive;
    uint16_t caps;
    uint32_t version;
    uint16_t flags;
    uint16_t type;
    uint16_t verhi;
    uint16_t verlo;
    uint32_t versub;
    uint32_t plangs;
    uint32_t pfiles;
    uint32_t pdeps;
    uint32_t pcerts;
    uint32_t pnames;
    uint32_t psig;
    uint32_t pcaps;
    uint32_t uspace;
    uint32_t nspace;
  } sis;
  const char **alangs;
  uint16_t *llangs;
  unsigned int i, sleft=0, smax=0, umped=0;
  uint8_t compd, buff[BUFSIZ];

  if (fread(&sis,sizeof(sis),1,f)!=1) {
    cli_dbgmsg("SIS: Unable to read header\n");
    return CL_CLEAN;
  }
  /*  cli_dbgmsg("SIS HEADER INFO: \nFile checksum: %x\nLangs: %d\nFiles: %d\nDeps: %d\nUsed langs: %d\nInstalled files: %d\nDest drive: %d\nCapabilities: %d\nSIS Version: %d\nFlags: %x\nType: %d\nVersion: %d.%d.%d\nLangs@: %x\nFiles@: %x\nDeps@: %x\nCerts@: %x\nName@: %x\nSig@: %x\nCaps@: %x\nUspace: %d\nNspace: %d\n\n", sis.filesum, sis.langs, sis.files, sis.deps, sis.ulangs, sis.instfiles, sis.drive, sis.caps, sis.version, sis.flags, sis.type, sis.verhi, sis.verlo, sis.versub, sis.plangs, sis.pfiles, sis.pdeps, sis.pcerts, sis.pnames, sis.psig, sis.pcaps, sis.uspace, sis.nspace);
   */

#if WORDS_BIGENDIAN != 0
  sis.langs=EC16(sis.langs);
  sis.files=EC16(sis.files);
  sis.deps=EC16(sis.deps);
  sis.flags=EC16(sis.flags);
  sis.plangs=EC32(sis.plangs);
  sis.pfiles=EC32(sis.pfiles);
  sis.pdeps=EC32(sis.pdeps);
  sis.pnames=EC32(sis.pnames);
  sis.pcaps=EC32(sis.pcaps);
#endif

  if (!sis.langs || sis.langs>=MAXLANG) {
    cli_dbgmsg("SIS: Too many or too few languages found\n");
    return CL_CLEAN;
  }
  fseek(f, sis.plangs, SEEK_SET);
  if (!(llangs=cli_malloc(sis.langs * sizeof(uint16_t)))) {
    cli_dbgmsg("SIS: OOM\n");
    return CL_CLEAN;
  }
  if (fread(llangs, sis.langs * sizeof(uint16_t), 1, f)!=1) {
    cli_dbgmsg("SIS: Unable to read languages\n");
    free(llangs);
    return CL_CLEAN;
  }
  if (!(alangs=cli_malloc(sis.langs * sizeof(char *)))) {
    cli_dbgmsg("SIS: OOM\n");
    free(llangs);
    return CL_CLEAN;
  }
  for (i = 0; i< sis.langs; i++)
    alangs[i]=EC32(llangs[i])<MAXLANG ? sislangs[EC32(llangs[i])] : sislangs[0];
  free(llangs);

  if (!sis.pnames) {
    cli_dbgmsg("SIS: Application without a name?\n");
  } else {
    fseek(f, sis.pnames, SEEK_SET);
    cli_dbgmsg("SIS: Application name:\n");
    if (!spamsisnames(f, sis.langs, alangs)) {
      free(alangs);
      return CL_EMEM;
    }
  }

  if (!sis.pcaps) {
    cli_dbgmsg("SIS: Application without capabilities?\n");
  } else {
    fseek(f, sis.pcaps, SEEK_SET);
    cli_dbgmsg("SIS: Provides:\n");
    if (!spamsisnames(f, sis.langs, alangs)) {
      free(alangs);
      return CL_EMEM;
    }
  }

  if (!sis.pdeps) {
    cli_dbgmsg("SIS: No dependencies set for this application\n");
  } else {
    cli_dbgmsg("SIS: Depends on:\n");
    for (i = 0; i< sis.deps; i++) {
      struct {
	uint32_t uid;
	uint16_t verhi;
	uint16_t verlo;
	uint32_t versub;
      } dep;
      
      fseek(f, sis.pdeps + i*(sizeof(dep) + sis.langs*2*sizeof(uint32_t)), SEEK_SET);
      if (fread(&dep, sizeof(dep), 1, f)!=1) {
	cli_dbgmsg("SIS: Unable to read dependencies\n");
      } else {
	cli_dbgmsg("\tUID: %x v. %d.%d.%d\n\taka:\n", EC32(dep.uid), EC16(dep.verhi), EC16(dep.verlo), EC32(dep.versub));
	if (!spamsisnames(f, sis.langs, alangs)) {
	  free(alangs);
	  return CL_EMEM;
	}
      }
    }
  }

  compd = !(sis.flags & 0x0008);
  cli_dbgmsg("SIS: Package is%s compressed\n", (compd)?"":" not");

  fseek(f, sis.pfiles, SEEK_SET);
  for (i=0; i<sis.files; i++) {
    uint32_t pkgtype, fcount=1;
    uint32_t j;

    GETD(pkgtype);
    cli_dbgmsg("SIS: Pkgtype: %d\n", pkgtype);
    switch(pkgtype) {
    case PKGlangfile:
      fcount=sis.langs;
    case PKGfile: {
      uint32_t ftype,options,ssname,psname,sdname,pdname;
      const char *sftype;
      uint32_t *ptrs, *lens, *olens;
      char *fn;
      long fpos;

      GETD(ftype);
      GETD(options);
      GETD(ssname);
      GETD(psname);
      GETD(sdname);
      GETD(pdname);
      switch(ftype) {
      case FTsimple:
	sftype = "simple";
	break;
      case FTtext:
	sftype = "text";
	break;
      case FTcomponent:
	sftype = "component";
	break;
      case FTrun:
	sftype = "run";
	break;
      case FTnull:
	sftype = "null";
	break;
      case FTmime:
	sftype = "mime";
	break;
      case FTsubsis:
	sftype = "sub sis";
	break;
      case FTcontsis:
	sftype = "conatiner sis";
	break;
      case FTtextuninst:
	sftype = "uninstall text";
	break;
      case FTnotinst:
	sftype = "not to be intalled";
	break;
      default:
	sftype = "unknown";
      }
      cli_dbgmsg("SIS: File details:\n\tOptions: %d\n\tType: %s\n", options, sftype);
      fpos = ftell(f);
      if ((fn=getsistring(f, psname, ssname))) {
	cli_dbgmsg("\tOriginal filename: %s\n", fn);
	free(fn);
      }
      if ((fn=getsistring(f, pdname, sdname))) {
	cli_dbgmsg("\tInstalled to: %s\n", fn);
	free(fn);
      }
      fseek(f,fpos,SEEK_SET);

      if (!(ptrs=cli_malloc(fcount*sizeof(uint32_t)*3))) {
	cli_dbgmsg("\tOOM\n");
	free(alangs);
	return CL_CLEAN;
      }
      lens=&ptrs[fcount];
      olens=&ptrs[fcount*2];
      for (j=0; j<fcount; j++)
	GETD2(lens[j]);
      for (j=0; j<fcount; j++)
	GETD2(ptrs[j]);
      for (j=0; j<fcount; j++)
	GETD2(olens[j]);
      
      if (ftype!=FTnull) {
	char ofn[1024];
	int fd;

	fpos = ftell(f);

	for (j=0; j<fcount; j++) {
	  void *comp, *decomp;
	  uLongf olen;

	  if (!lens[j]) {
	    cli_dbgmsg("\tSkipping empty file\n");
	    continue;
	  }
	  if (cli_checklimits("sis", ctx,lens[j], 0, 0)!=CL_CLEAN) continue;
	  cli_dbgmsg("\tUnpacking lang#%d - ptr:%x csize:%x osize:%x\n", j, ptrs[j], lens[j], olens[j]);
	  if (!(comp=cli_malloc(lens[j]))) {
	    cli_dbgmsg("\tOOM\n");
	    free(ptrs);
	    free(alangs);
	    return CL_CLEAN;
	  }
	  fseek(f,ptrs[j],SEEK_SET);
	  if (fread(comp, lens[j], 1, f)!=1) {
	    cli_dbgmsg("\tSkipping ghost or otherwise out of archive file\n");
	    free(comp);
	    continue;
	  }
	  if (compd) {
	    if (olens[j]<=lens[j]*3 && cli_checklimits("sis", ctx, lens[j]*3, 0, 0)==CL_CLEAN)
	      olen=lens[j]*3;
	    else if (cli_checklimits("sis", ctx, olens[j], 0, 0)==CL_CLEAN)
	      olen=olens[j];
	    else {
	      free(comp);
	      continue;
	    }
	      
	    if (!(decomp=cli_malloc(olen))) {
	      cli_dbgmsg("\tOOM\n");
	      free(comp);
	      free(ptrs);
	      free(alangs);
	      return CL_CLEAN;
	    }
	    if (uncompress(decomp, &olen, comp, lens[j])!=Z_OK) {
	      cli_dbgmsg("\tUnpacking failure\n");
	      free(comp);
	      free(decomp);
	      continue;
	    }
	    free(comp);
	  } else {
	    olen = lens[j];
	    decomp = comp;
	  }
	  snprintf(ofn, 1024, "%s"PATHSEP"sis%02d", tmpd, umped);
	  ofn[1023]='\0';
	  if ((fd=open(ofn, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0600))==-1) {
	    cli_errmsg("SIS: unable to create output file %s - aborting.", ofn);
	    free(decomp);
	    free(ptrs);
	    free(alangs);
	    return CL_ECREAT;
	  }
	  if (cli_writen(fd, decomp, olen)!=(int)olen) {
	    close(fd);
	    free(decomp);
	    free(ptrs);
	    free(alangs);
	    return CL_EWRITE;
	  }
	  free(decomp);
	  if (cli_magic_scandesc(fd, ctx) == CL_VIRUS) {
	    close(fd);
	    free(ptrs);
	    free(alangs);
	    return CL_VIRUS;
	  }
	  close(fd);
	  umped++;
	}
	fseek(f,fpos,SEEK_SET);
      }
      free(ptrs);
      fcount=2*sizeof(uint32_t);
      break;
    }
    case PKGoption:
      cli_dbgmsg("SIS: I'm an option\n");
      GETD(fcount);
      fcount*=sis.langs*2*sizeof(uint32_t);
      break;
    case PKGif:
      cli_dbgmsg("SIS: #if\n");
      GETD(fcount);
      break;
    case PKGelsif:
      cli_dbgmsg("SIS: #elsif\n");
      GETD(fcount);
      break;
    case PKGelse:
      cli_dbgmsg("SIS: #else\n");
      fcount=0;
      break;
    case PKGendif:
      cli_dbgmsg("SIS: #endif\n");
      fcount=0;
      break;
    default:
      cli_dbgmsg("SIS: Unknown PKGtype, expect troubles\n");
      fcount=0;
    }
    SKIP(fcount);
  }

  free(alangs);
  return CL_CLEAN;
}
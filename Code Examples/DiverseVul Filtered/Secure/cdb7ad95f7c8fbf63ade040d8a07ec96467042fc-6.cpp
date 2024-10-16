int FoFiTrueType::setupGSUB(const char *scriptName,
                            const char *languageName)
{
  Guint gsubTable;
  unsigned int i;
  Guint scriptList, featureList;
  Guint scriptCount;
  Guint tag;
  Guint scriptTable = 0;
  Guint langSys;
  Guint featureCount;
  Guint featureIndex;
  Guint ftable = 0;
  Guint llist;
  Guint scriptTag;
  int x;
  Guint pos;

  if (scriptName == 0) {
    gsubFeatureTable = 0;
    return 0;
  }
  scriptTag = charToTag(scriptName);
  /* read GSUB Header */
  if ((x = seekTable("GSUB")) < 0) {
    return 0; /* GSUB table not found */
  }
  gsubTable = tables[x].offset;
  pos = gsubTable+4;
  scriptList = getU16BE(pos,&parsedOk);
  pos += 2;
  featureList = getU16BE(pos,&parsedOk);
  pos += 2;
  llist = getU16BE(pos,&parsedOk);

  gsubLookupList = llist+gsubTable; /* change to offset from top of file */
  /* read script list table */
  pos = gsubTable+scriptList;
  scriptCount = getU16BE(pos,&parsedOk);
  pos += 2;
  /* find  script */
  for (i = 0;i < scriptCount;i++) {
    tag = getU32BE(pos,&parsedOk);
    pos += 4;
    scriptTable = getU16BE(pos,&parsedOk);
    pos += 2;
    if (tag == scriptTag) {
      /* found */
      break;
    }
  }
  if (i >= scriptCount) {
    /* not found */
    return 0;
  }

  /* read script table */
  /* use default language system */
  pos = gsubTable+scriptList+scriptTable;
  langSys = 0;
  if (languageName) {
    Guint langTag = charToTag(languageName);
    Guint langCount = getU16BE(pos+2,&parsedOk);
    for (i = 0;i < langCount && langSys == 0;i++) {
      tag = getU32BE(pos+4+i*(4+2),&parsedOk);
      if (tag == langTag) {
        langSys = getU16BE(pos+4+i*(4+2)+4,&parsedOk);
      }
    }
  }
  if (langSys == 0) {
    /* default language system */
    langSys = getU16BE(pos,&parsedOk);
  }

  /* read LangSys table */
  if (langSys == 0) {
    /* no default LangSys */
    return 0;
  }

  pos = gsubTable+scriptList+scriptTable+langSys+2;
  featureIndex = getU16BE(pos,&parsedOk); /* ReqFeatureIndex */
  pos += 2;

  if (featureIndex != 0xffff) {
    Guint tpos;
    /* read feature record */
    tpos = gsubTable+featureList;
    featureCount = getU16BE(tpos,&parsedOk);
    tpos = gsubTable+featureList+2+featureIndex*(4+2);
    tag = getU32BE(tpos,&parsedOk);
    tpos += 4;
    if (tag == vrt2Tag) {
      /* vrt2 is preferred, overwrite vert */
      ftable = getU16BE(tpos,&parsedOk);
      /* convert to offset from file top */
      gsubFeatureTable = ftable+gsubTable+featureList;
      return 0;
    } else if (tag == vertTag) {
      ftable = getU16BE(tpos,&parsedOk);
    }
  }
  featureCount = getU16BE(pos,&parsedOk);
  pos += 2;
  /* find 'vrt2' or 'vert' feature */
  for (i = 0;i < featureCount;i++) {
    Guint oldPos;

    featureIndex = getU16BE(pos,&parsedOk);
    pos += 2;
    oldPos = pos; /* save position */
    /* read feature record */
    pos = gsubTable+featureList+2+featureIndex*(4+2);
    tag = getU32BE(pos,&parsedOk);
    pos += 4;
    if (tag == vrt2Tag) {
      /* vrt2 is preferred, overwrite vert */
      ftable = getU16BE(pos,&parsedOk);
      break;
    } else if (ftable == 0 && tag == vertTag) {
      ftable = getU16BE(pos,&parsedOk);
    }
    pos = oldPos; /* restore old position */
  }
  if (ftable == 0) {
    /* vert nor vrt2 are not found */
    return 0;
  }
  /* convert to offset from file top */
  gsubFeatureTable = ftable+gsubTable+featureList;
  return 0;
}
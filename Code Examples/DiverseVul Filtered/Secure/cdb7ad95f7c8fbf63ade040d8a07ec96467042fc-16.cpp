Guint FoFiTrueType::doMapToVertGID(Guint orgGID)
{
  Guint lookupCount;
  Guint lookupListIndex;
  Guint i;
  Guint gid = 0;
  Guint pos;

  pos = gsubFeatureTable+2;
  lookupCount = getU16BE(pos,&parsedOk);
  pos += 2;
  for (i = 0;i < lookupCount;i++) {
    lookupListIndex = getU16BE(pos,&parsedOk);
    pos += 2;
    if ((gid = scanLookupList(lookupListIndex,orgGID)) != 0) {
      break;
    }
  }
  return gid;
}
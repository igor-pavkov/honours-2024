Guint FoFiTrueType::scanLookupList(Guint listIndex, Guint orgGID)
{
  Guint lookupTable;
  Guint subTableCount;
  Guint subTable;
  Guint i;
  Guint gid = 0;
  Guint pos;

  if (gsubLookupList == 0) return 0; /* no lookup list */
  pos = gsubLookupList+2+listIndex*2;
  lookupTable = getU16BE(pos,&parsedOk);
  /* read lookup table */
  pos = gsubLookupList+lookupTable+4;
  subTableCount = getU16BE(pos,&parsedOk);
  pos += 2;;
  for (i = 0;i < subTableCount;i++) {
    subTable = getU16BE(pos,&parsedOk);
    pos += 2;
    if ((gid = scanLookupSubTable(gsubLookupList+lookupTable+subTable,orgGID))
         != 0) break;
  }
  return gid;
}
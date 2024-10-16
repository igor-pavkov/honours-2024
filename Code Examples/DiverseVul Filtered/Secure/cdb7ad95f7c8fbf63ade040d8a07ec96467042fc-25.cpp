FoFiTrueType::FoFiTrueType(char *fileA, int lenA, GBool freeFileDataA, int faceIndexA):
  FoFiBase(fileA, lenA, freeFileDataA)
{
  tables = NULL;
  nTables = 0;
  cmaps = NULL;
  nCmaps = 0;
  nameToGID = NULL;
  parsedOk = gFalse;
  faceIndex = faceIndexA;
  gsubFeatureTable = 0;
  gsubLookupList = 0;

  parse();
}
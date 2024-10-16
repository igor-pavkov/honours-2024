int main (int argc, char *argv[]) {
  if (argc < 4) {
    printf("USAGE: shpsort <infile> <outfile> <field[;...]> [<(ASCENDING|DESCENDING)[;...]>]\n");
    exit(EXIT_FAILURE);
  }

  SHPHandle inSHP = SHPOpen (argv[1], "rb");
  if (!inSHP) {
    fputs("Couldn't open shapefile for reading!\n", stderr);
    exit(EXIT_FAILURE);
  }
  SHPGetInfo(inSHP, &nShapes, &shpType, NULL, NULL);

  /* If we can open the inSHP, open its DBF */
  DBFHandle inDBF = DBFOpen (argv[1], "rb");
  if (!inDBF) {
    fputs("Couldn't open dbf file for reading!\n", stderr);
    exit(EXIT_FAILURE);
  }

  /* Parse fields and validate existence */
  char **fieldNames = split(argv[3], ";");
  if (!fieldNames) {
    fputs("ERROR: parsing field names!\n", stderr);
    exit(EXIT_FAILURE);
  }
  for (nFields = 0; fieldNames[nFields] ; nFields++) {
    continue;
  }

  fldIdx = malloc(sizeof *fldIdx * nFields);
  if (!fldIdx) {
    fputs("malloc failed!\n", stderr);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < nFields; i++) {
    int len = (int)strlen(fieldNames[i]);
    while(len > 0) {
      --len;
      fieldNames[i][len] = (char)toupper((unsigned char)fieldNames[i][len]);
    }
    fldIdx[i] = DBFGetFieldIndex(inDBF, fieldNames[i]);
    if (fldIdx[i] < 0) {
      /* try "SHAPE" */
      if (strcmp(fieldNames[i], "SHAPE") == 0) {
	fldIdx[i] = -1;
      }
      else if (strcmp(fieldNames[i], "FID") == 0) {
	fldIdx[i] = -2;
      }
      else {
	fprintf(stderr, "ERROR: field '%s' not found!\n", fieldNames[i]);
	exit(EXIT_FAILURE);
      }
    }
  }

  /* set up field type array */
  fldType = malloc(sizeof *fldType * nFields);
  if (!fldType) {
    fputs("malloc failed!\n", stderr);
    exit(EXIT_FAILURE);
  }
  int width;
  int decimals;
  for (int i = 0; i < nFields; i++) {
    if (fldIdx[i] < 0) {
      fldType[i] = fldIdx[i];
    }
    else {
      fldType[i] = DBFGetFieldInfo(inDBF, fldIdx[i], NULL, &width, &decimals);
      if (fldType[i] == FTInvalid) {
	fputs("Unrecognized field type in dBASE file!\n", stderr);
	exit(EXIT_FAILURE);
      }
    }
  }

  /* set up field order array */
  fldOrder = malloc(sizeof *fldOrder * nFields);
  if (!fldOrder) {
    fputs("malloc failed!\n", stderr);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < nFields; i++) {
    /* default to ascending order */
    fldOrder[i] = ASCENDING;
  }
  if (argc > 4) {
    char **strOrder = split(argv[4], ";");
    if (!strOrder) {
      fputs("ERROR: parsing fields ordering!\n", stderr);
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nFields && strOrder[i]; i++) {
      if (strcmp(strOrder[i], "DESCENDING") == 0) {
	fldOrder[i] = DESCENDING;
      }
    }
  }

  /* build the index */
  struct DataStruct *index = build_index (inSHP, inDBF);

  /* Create output shapefile */
  SHPHandle outSHP = SHPCreate(argv[2], shpType);
  if (!outSHP) {
    fprintf(stderr, "%s:%d: couldn't create output shapefile!\n",
	    __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }

  /* Create output dbf */
  DBFHandle outDBF = DBFCloneEmpty(inDBF, argv[2]);
  if (!outDBF) {
    fprintf(stderr, "%s:%d: couldn't create output dBASE file!\n",
	    __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }

  /* Copy projection file, if any */
  copy_related(argv[1], argv[2], ".shp", ".prj");

  /* Copy metadata file, if any */
  copy_related(argv[1], argv[2], ".shp", ".shp.xml");

  /* Write out sorted results */
  for (int i = 0; i < nShapes; i++) {
    SHPObject *feat = SHPReadObject(inSHP, index[i].record);
    if (SHPWriteObject(outSHP, -1, feat) < 0) {
      fprintf(stderr, "%s:%d: error writing shapefile!\n", __FILE__, __LINE__);
      exit(EXIT_FAILURE);
    }
    void *tuple = (void *) DBFReadTuple(inDBF, index[i].record);
    if (DBFWriteTuple(outDBF, i, tuple) < 0) {
      fprintf(stderr, "%s:%d: error writing dBASE file!\n", __FILE__, __LINE__);
      exit(EXIT_FAILURE);
    }
  }
  SHPClose(inSHP);
  SHPClose(outSHP);
  DBFClose(inDBF);
  DBFClose(outDBF);

  return EXIT_SUCCESS;
}
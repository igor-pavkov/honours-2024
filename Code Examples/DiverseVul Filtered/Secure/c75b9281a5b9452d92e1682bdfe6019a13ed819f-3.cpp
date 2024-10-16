static struct DataStruct * build_index (SHPHandle shp, DBFHandle dbf) {
  struct DataStruct *data = malloc (sizeof *data * nShapes);
  if (!data) {
    fputs("malloc failed!\n", stderr);
    exit(EXIT_FAILURE);
  }

  /* populate array */
  for (int i = 0; i < nShapes; i++) {
    data[i].value = malloc(sizeof data[0].value[0] * nFields);
    if (0 == data[i].value) {
      fputs("malloc failed!\n", stderr);
      exit(EXIT_FAILURE);
    }
    data[i].record = i;
    for (int j = 0; j < nFields; j++) {
      data[i].value[j].null = 0;
      switch (fldType[j]) {
      case FIDType:
	data[i].value[j].u.i = i;
	break;
      case SHPType:
       {
        SHPObject *feat = SHPReadObject(shp, i);
	switch (feat->nSHPType) {
	case SHPT_NULL:
	  fprintf(stderr, "Shape %d is a null feature!\n", i);
	  data[i].value[j].null = 1;
	  break;
	case SHPT_POINT:
	case SHPT_POINTZ:
	case SHPT_POINTM:
	case SHPT_MULTIPOINT:
	case SHPT_MULTIPOINTZ:
	case SHPT_MULTIPOINTM:
	case SHPT_MULTIPATCH:
	  /* Y-sort bounds */
	  data[i].value[j].u.d = feat->dfYMax;
	  break;
	case SHPT_ARC:
	case SHPT_ARCZ:
	case SHPT_ARCM:
	  data[i].value[j].u.d = shp_length(feat);
	  break;
	case SHPT_POLYGON:
	case SHPT_POLYGONZ:
	case SHPT_POLYGONM:
	  data[i].value[j].u.d = shp_area(feat);
	  break;
	default:
	  fputs("Can't sort on Shapefile feature type!\n", stderr);
	  exit(EXIT_FAILURE);
	}
	SHPDestroyObject(feat);
	break;
       }
      case FTString:
	data[i].value[j].null = DBFIsAttributeNULL(dbf, i, fldIdx[j]);
	if (!data[i].value[j].null) {
	  data[i].value[j].u.s = dupstr(DBFReadStringAttribute(dbf, i, fldIdx[j]));
	}
	break;
      case FTInteger:
      case FTLogical:
	data[i].value[j].null = DBFIsAttributeNULL(dbf, i, fldIdx[j]);
	if (!data[i].value[j].null) {
	  data[i].value[j].u.i  = DBFReadIntegerAttribute(dbf, i, fldIdx[j]);
	}
	break;
      case FTDouble:
	data[i].value[j].null = DBFIsAttributeNULL(dbf, i, fldIdx[j]);
	if (!data[i].value[j].null) {
	  data[i].value[j].u.d = DBFReadDoubleAttribute(dbf, i, fldIdx[j]);
	}
	break;
      }
    }
  }

#ifdef DEBUG
  PrintDataStruct(data);
  fputs("build_index: sorting array\n", stdout);
#endif

  qsort (data, nShapes, sizeof data[0], compare);

#ifdef DEBUG
  PrintDataStruct(data);
  fputs("build_index: returning array\n", stdout);
#endif

  return data;
}
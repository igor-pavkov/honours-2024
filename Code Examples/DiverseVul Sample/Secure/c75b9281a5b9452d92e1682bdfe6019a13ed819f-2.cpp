static int compare(const void *A, const void *B) {
  const struct DataStruct *a = A;
  const struct DataStruct *b = B;

  for (int i = 0; i < nFields; i++) {
    if (a->value[i].null && b->value[i].null) {
      continue;
    }
    if (a->value[i].null && !b->value[i].null) {
      return (fldOrder[i]) ? 1 : -1;
    }
    if (!a->value[i].null && b->value[i].null) {
      return (fldOrder[i]) ? -1 : 1;
    }
    switch (fldType[i]) {
    case FIDType:
    case IntegerType:
    case LogicalType:
      if (a->value[i].u.i < b->value[i].u.i) {
	return (fldOrder[i]) ? -1 : 1;
      }
      if (a->value[i].u.i > b->value[i].u.i) {
	return (fldOrder[i]) ? 1 : -1;
      }
      break;
    case DoubleType:
    case SHPType:
      if (a->value[i].u.d < b->value[i].u.d) {
	return (fldOrder[i]) ? -1 : 1;
      }
      if (a->value[i].u.d > b->value[i].u.d) {
	return (fldOrder[i]) ? 1 : -1;
      }
      break;
    case StringType:
     {
      const int result = strcmp(a->value[i].u.s, b->value[i].u.s);
      if (result) {
	return (fldOrder[i]) ? result : -result;
      }
      break;
     }
    default:
      fprintf(stderr, "compare: Program Error! Unhandled field type! fldType[%d] = %d\n", i, fldType[i]);
      break;
    }
  }
  return 0;
}
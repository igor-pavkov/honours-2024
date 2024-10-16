static void PrintDataStruct (struct DataStruct *data) {
  for (int i = 0; i < nShapes; i++) {
    printf("data[%d] {\n", i);
    printf("\t.record = %d\n", data[i].record);
    for (int j = 0; j < nFields; j++) {
      printf("\t.value[%d].null = %d\n", j, data[i].value[j].null);
      if (!data[i].value[j].null) {
	switch(fldType[j]) {
	case FIDType:
	case IntegerType:
	case LogicalType:
	  printf("\t.value[%d].u.i = %d\n", j, data[i].value[j].u.i);
	  break;
	case DoubleType:
	case SHPType:
	  printf("\t.value[%d].u.d = %f\n", j, data[i].value[j].u.d);
	  break;
	case StringType:
	  printf("\t.value[%d].u.s = %s\n", j, data[i].value[j].u.s);
	  break;
	}
      }
    }
    puts("}");
  }
}
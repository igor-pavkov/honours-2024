int readData(struct READER *reader, struct DATAOBJECT *da, struct DATATYPE *dt,
             struct DATASPACE *ds) {
  if (ds->dimensionality == 0) {
    ds->dimension_size[0] = 1;
  }
  return readDataDim(reader, da, dt, ds, 0);
}
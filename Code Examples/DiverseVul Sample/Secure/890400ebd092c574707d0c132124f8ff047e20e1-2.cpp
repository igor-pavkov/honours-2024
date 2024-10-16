void dataobjectFree(struct READER *reader, struct DATAOBJECT *dataobject) {
  struct DATAOBJECT **p;

  btreeFree(&dataobject->attributes_btree);
  fractalheapFree(&dataobject->attributes_heap);
  btreeFree(&dataobject->objects_btree);
  fractalheapFree(&dataobject->objects_heap);

  while (dataobject->attributes) {
    struct MYSOFA_ATTRIBUTE *attr = dataobject->attributes;
    dataobject->attributes = attr->next;
    free(attr->name);
    free(attr->value);
    free(attr);
  }

  while (dataobject->directory) {
    struct DIR *dir = dataobject->directory;
    dataobject->directory = dir->next;
    dataobjectFree(reader, &dir->dataobject);
    free(dir);
  }

  free(dataobject->data);
  free(dataobject->string);
  free(dataobject->name);

  p = &reader->all;
  while (*p) {
    if ((*p) == dataobject) {
      *p = dataobject->all;
      break;
    }
    p = &((*p)->all);
  }
}
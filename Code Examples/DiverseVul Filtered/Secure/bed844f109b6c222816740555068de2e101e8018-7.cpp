void jslCharPosFree(JslCharPos *pos) {
  jsvStringIteratorFree(&pos->it);
}
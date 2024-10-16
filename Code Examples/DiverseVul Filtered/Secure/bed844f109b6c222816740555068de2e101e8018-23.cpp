JslCharPos jslCharPosClone(JslCharPos *pos) {
  JslCharPos p;
  p.it = jsvStringIteratorClone(&pos->it);
  p.currCh = pos->currCh;
  return p;
}
Goffset PDFDoc::writeObjectHeader (Ref *ref, OutStream* outStr)
{
  Goffset offset = outStr->getPos();
  outStr->printf("%i %i obj\r\n", ref->num, ref->gen);
  return offset;
}
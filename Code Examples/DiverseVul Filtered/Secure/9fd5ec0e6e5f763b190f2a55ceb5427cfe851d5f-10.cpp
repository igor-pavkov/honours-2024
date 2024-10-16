void PDFDoc::writeXRefTableTrailer(Object &&trailerDict, XRef *uxref, bool writeAllEntries, Goffset uxrefOffset, OutStream* outStr, XRef *xRef)
{
  uxref->writeTableToFile( outStr, writeAllEntries );
  outStr->printf( "trailer\r\n");
  writeDictionnary(trailerDict.getDict(), outStr, xRef, 0, nullptr, cryptRC4, 0, 0, 0, nullptr);
  outStr->printf( "\r\nstartxref\r\n");
  outStr->printf( "%lli\r\n", uxrefOffset);
  outStr->printf( "%%%%EOF\r\n");
}
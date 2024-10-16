void PDFDoc::writeHeader(OutStream *outStr, int major, int minor)
{
   outStr->printf("%%PDF-%d.%d\n", major, minor);
   outStr->printf("%%%c%c%c%c\n", 0xE2, 0xE3, 0xCF, 0xD3);
}
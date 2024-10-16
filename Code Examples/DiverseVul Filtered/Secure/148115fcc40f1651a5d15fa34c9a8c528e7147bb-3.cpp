XML_Parser OGRCreateExpatXMLParser()
{
    XML_Memory_Handling_Suite memsuite;
    memsuite.malloc_fcn = OGRExpatMalloc;
    memsuite.realloc_fcn = OGRExpatRealloc;
    memsuite.free_fcn = free;
    XML_Parser hParser = XML_ParserCreate_MM(nullptr, &memsuite, nullptr);

    XML_SetUnknownEncodingHandler(hParser,
                                  OGRExpatUnknownEncodingHandler,
                                  nullptr);

    return hParser;
}
static int OGRExpatUnknownEncodingHandler(
    void * /* unused_encodingHandlerData */,
    const XML_Char *name,
    XML_Encoding *info )
{
    if( EQUAL(name, "WINDOWS-1252") )
        FillWINDOWS1252(info);
    else if( EQUAL(name, "ISO-8859-15") )
        FillISO885915(info);
    else
    {
        CPLDebug("OGR", "Unhandled encoding %s", name);
        return XML_STATUS_ERROR;
    }

    info->data    = nullptr;
    info->convert = nullptr;
    info->release = nullptr;

    return XML_STATUS_OK;
}
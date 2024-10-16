static void showVersion(const char *name) {
    fprintf(stderr, "%s: using libxml version %s\n", name, xmlParserVersion);
    fprintf(stderr, "   compiled with: ");
    if (xmlHasFeature(XML_WITH_THREAD)) fprintf(stderr, "Threads ");
    if (xmlHasFeature(XML_WITH_TREE)) fprintf(stderr, "Tree ");
    if (xmlHasFeature(XML_WITH_OUTPUT)) fprintf(stderr, "Output ");
    if (xmlHasFeature(XML_WITH_PUSH)) fprintf(stderr, "Push ");
    if (xmlHasFeature(XML_WITH_READER)) fprintf(stderr, "Reader ");
    if (xmlHasFeature(XML_WITH_PATTERN)) fprintf(stderr, "Patterns ");
    if (xmlHasFeature(XML_WITH_WRITER)) fprintf(stderr, "Writer ");
    if (xmlHasFeature(XML_WITH_SAX1)) fprintf(stderr, "SAXv1 ");
    if (xmlHasFeature(XML_WITH_FTP)) fprintf(stderr, "FTP ");
    if (xmlHasFeature(XML_WITH_HTTP)) fprintf(stderr, "HTTP ");
    if (xmlHasFeature(XML_WITH_VALID)) fprintf(stderr, "DTDValid ");
    if (xmlHasFeature(XML_WITH_HTML)) fprintf(stderr, "HTML ");
    if (xmlHasFeature(XML_WITH_LEGACY)) fprintf(stderr, "Legacy ");
    if (xmlHasFeature(XML_WITH_C14N)) fprintf(stderr, "C14N ");
    if (xmlHasFeature(XML_WITH_CATALOG)) fprintf(stderr, "Catalog ");
    if (xmlHasFeature(XML_WITH_XPATH)) fprintf(stderr, "XPath ");
    if (xmlHasFeature(XML_WITH_XPTR)) fprintf(stderr, "XPointer ");
    if (xmlHasFeature(XML_WITH_XINCLUDE)) fprintf(stderr, "XInclude ");
    if (xmlHasFeature(XML_WITH_ICONV)) fprintf(stderr, "Iconv ");
    if (xmlHasFeature(XML_WITH_ICU)) fprintf(stderr, "ICU ");
    if (xmlHasFeature(XML_WITH_ISO8859X)) fprintf(stderr, "ISO8859X ");
    if (xmlHasFeature(XML_WITH_UNICODE)) fprintf(stderr, "Unicode ");
    if (xmlHasFeature(XML_WITH_REGEXP)) fprintf(stderr, "Regexps ");
    if (xmlHasFeature(XML_WITH_AUTOMATA)) fprintf(stderr, "Automata ");
    if (xmlHasFeature(XML_WITH_EXPR)) fprintf(stderr, "Expr ");
    if (xmlHasFeature(XML_WITH_SCHEMAS)) fprintf(stderr, "Schemas ");
    if (xmlHasFeature(XML_WITH_SCHEMATRON)) fprintf(stderr, "Schematron ");
    if (xmlHasFeature(XML_WITH_MODULES)) fprintf(stderr, "Modules ");
    if (xmlHasFeature(XML_WITH_DEBUG)) fprintf(stderr, "Debug ");
    if (xmlHasFeature(XML_WITH_DEBUG_MEM)) fprintf(stderr, "MemDebug ");
    if (xmlHasFeature(XML_WITH_DEBUG_RUN)) fprintf(stderr, "RunDebug ");
    if (xmlHasFeature(XML_WITH_ZLIB)) fprintf(stderr, "Zlib ");
    if (xmlHasFeature(XML_WITH_LZMA)) fprintf(stderr, "Lzma ");
    fprintf(stderr, "\n");
}
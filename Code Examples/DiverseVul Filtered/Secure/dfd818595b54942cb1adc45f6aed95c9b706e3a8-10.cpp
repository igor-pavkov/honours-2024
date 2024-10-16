AnyP::Uri::parseUrn(Parser::Tokenizer &tok)
{
    static const auto nidChars = CharacterSet("NID","-") + CharacterSet::ALPHA + CharacterSet::DIGIT;
    static const auto alphanum = (CharacterSet::ALPHA + CharacterSet::DIGIT).rename("alphanum");
    SBuf nid;
    if (!tok.prefix(nid, nidChars, 32))
        throw TextException("NID not found", Here());

    if (!tok.skip(':'))
        throw TextException("NID too long or missing ':' delimiter", Here());

    if (nid.length() < 2)
        throw TextException("NID too short", Here());

    if (!alphanum[*nid.begin()])
        throw TextException("NID prefix is not alphanumeric", Here());

    if (!alphanum[*nid.rbegin()])
        throw TextException("NID suffix is not alphanumeric", Here());

    setScheme(AnyP::PROTO_URN, nullptr);
    host(nid.c_str());
    // TODO validate path characters
    path(tok.remaining());
    debugs(23, 3, "Split URI into proto=urn, nid=" << nid << ", " << Raw("path",path().rawContent(),path().length()));
}
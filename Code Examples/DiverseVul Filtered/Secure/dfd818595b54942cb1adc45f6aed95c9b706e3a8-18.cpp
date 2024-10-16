AnyP::Uri::Encode(const SBuf &buf, const CharacterSet &ignore)
{
    if (buf.isEmpty())
        return buf;

    Parser::Tokenizer tk(buf);
    SBuf goodSection;
    // optimization for the arguably common "no encoding necessary" case
    if (tk.prefix(goodSection, ignore) && tk.atEnd())
        return buf;

    SBuf output;
    output.reserveSpace(buf.length() * 3); // worst case: encode all chars
    output.append(goodSection); // may be empty

    while (!tk.atEnd()) {
        // TODO: Add Tokenizer::parseOne(void).
        const auto ch = tk.remaining()[0];
        output.appendf("%%%02X", static_cast<unsigned int>(ch)); // TODO: Optimize using a table
        (void)tk.skip(ch);

        if (tk.prefix(goodSection, ignore))
            output.append(goodSection);
    }

    return output;
}
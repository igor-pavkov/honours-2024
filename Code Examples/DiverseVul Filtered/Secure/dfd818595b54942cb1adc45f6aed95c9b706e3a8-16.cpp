AnyP::Uri::cleanup(const char *uri)
{
    int flags = 0;
    char *cleanedUri = nullptr;
    switch (Config.uri_whitespace) {
    case URI_WHITESPACE_ALLOW:
        flags |= RFC1738_ESCAPE_NOSPACE;
    // fall through to next case
    case URI_WHITESPACE_ENCODE:
        flags |= RFC1738_ESCAPE_UNESCAPED;
        cleanedUri = xstrndup(rfc1738_do_escape(uri, flags), MAX_URL);
        break;

    case URI_WHITESPACE_CHOP: {
        flags |= RFC1738_ESCAPE_UNESCAPED;
        const auto pos = strcspn(uri, w_space);
        char *choppedUri = nullptr;
        if (pos < strlen(uri))
            choppedUri = xstrndup(uri, pos + 1);
        cleanedUri = xstrndup(rfc1738_do_escape(choppedUri ? choppedUri : uri, flags), MAX_URL);
        cleanedUri[pos] = '\0';
        xfree(choppedUri);
    }
    break;

    case URI_WHITESPACE_DENY:
    case URI_WHITESPACE_STRIP:
    default: {
        // TODO: avoid duplication with urlParse()
        const char *t;
        char *tmp_uri = static_cast<char*>(xmalloc(strlen(uri) + 1));
        char *q = tmp_uri;
        t = uri;
        while (*t) {
            if (!xisspace(*t)) {
                *q = *t;
                ++q;
            }
            ++t;
        }
        *q = '\0';
        cleanedUri = xstrndup(rfc1738_escape_unescaped(tmp_uri), MAX_URL);
        xfree(tmp_uri);
    }
    break;
    }

    assert(cleanedUri);
    return cleanedUri;
}
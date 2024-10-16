AnyP::Uri::path() const
{
    // RFC 3986 section 3.3 says path can be empty (path-abempty).
    // RFC 7230 sections 2.7.3, 5.3.1, 5.7.2 - says path cannot be empty, default to "/"
    // at least when sending and using. We must still accept path-abempty as input.
    if (path_.isEmpty() && (scheme_ == AnyP::PROTO_HTTP || scheme_ == AnyP::PROTO_HTTPS))
        return SlashPath();

    return path_;
}
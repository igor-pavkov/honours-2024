AnyP::Uri::addRelativePath(const char *relUrl)
{
    // URN cannot be merged
    if (getScheme() == AnyP::PROTO_URN)
        return;

    // TODO: Handle . and .. segment normalization

    const auto lastSlashPos = path_.rfind('/');
    // TODO: To optimize and simplify, add and use SBuf::replace().
    const auto relUrlLength = strlen(relUrl);
    if (lastSlashPos == SBuf::npos) {
        // start replacing the whole path
        path_.reserveCapacity(1 + relUrlLength);
        path_.assign("/", 1);
    } else {
        // start replacing just the last segment
        path_.reserveCapacity(lastSlashPos + 1 + relUrlLength);
        path_.chop(0, lastSlashPos+1);
    }
    path_.append(relUrl, relUrlLength);
}
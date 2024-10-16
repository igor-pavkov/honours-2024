AnyP::Uri::host(const char *src)
{
    hostAddr_.setEmpty();
    hostAddr_ = src;
    if (hostAddr_.isAnyAddr()) {
        xstrncpy(host_, src, sizeof(host_));
        hostIsNumeric_ = false;
    } else {
        hostAddr_.toHostStr(host_, sizeof(host_));
        debugs(23, 3, "given IP: " << hostAddr_);
        hostIsNumeric_ = 1;
    }
    touch();
}
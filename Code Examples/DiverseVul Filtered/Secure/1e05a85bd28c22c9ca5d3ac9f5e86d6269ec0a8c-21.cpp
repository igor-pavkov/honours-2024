Http::Stream::noteIoError(const int xerrno)
{
    if (http) {
        http->logType.err.timedout = (xerrno == ETIMEDOUT);
        // aborted even if xerrno is zero (which means read abort/eof)
        http->logType.err.aborted = (xerrno != ETIMEDOUT);
    }
}
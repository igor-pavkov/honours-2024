Client::serverComplete()
{
    debugs(11,5,HERE << "serverComplete " << this);

    if (!doneWithServer()) {
        closeServer();
        assert(doneWithServer());
    }

    completed = true;

    HttpRequest *r = originalRequest();
    r->hier.stopPeerClock(true);

    if (requestBodySource != NULL)
        stopConsumingFrom(requestBodySource);

    if (responseBodyBuffer != NULL)
        return;

    serverComplete2();
}
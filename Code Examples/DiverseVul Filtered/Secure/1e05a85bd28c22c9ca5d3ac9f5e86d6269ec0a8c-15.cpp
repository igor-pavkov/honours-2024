Client::~Client()
{
    // paranoid: check that swanSong has been called
    assert(!requestBodySource);
#if USE_ADAPTATION
    assert(!virginBodyDestination);
    assert(!adaptedBodySource);
#endif

    entry->unlock("Client");

    HTTPMSGUNLOCK(request);
    HTTPMSGUNLOCK(theVirginReply);
    HTTPMSGUNLOCK(theFinalReply);

    if (responseBodyBuffer != NULL) {
        delete responseBodyBuffer;
        responseBodyBuffer = NULL;
    }
}
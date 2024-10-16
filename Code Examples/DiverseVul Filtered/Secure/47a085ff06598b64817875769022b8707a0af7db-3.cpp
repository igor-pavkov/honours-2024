UrnState::start(HttpRequest * r, StoreEntry * e)
{
    debugs(52, 3, "urnStart: '" << e->url() << "'" );
    entry = e;
    request = r;

    entry->lock("UrnState::start");
    setUriResFromRequest(r);

    if (urlres_r == NULL)
        return;

    auto urlEntry = storeGetPublic(urlres, Http::METHOD_GET);

    if (!urlEntry || (urlEntry->hittingRequiresCollapsing() && !startCollapsingOn(*urlEntry, false))) {
        urlres_e = storeCreateEntry(urlres, urlres, RequestFlags(), Http::METHOD_GET);
        sc = storeClientListAdd(urlres_e, this);
        FwdState::Start(Comm::ConnectionPointer(), urlres_e, urlres_r.getRaw(), ale);
        if (urlEntry) {
            urlEntry->abandon(__FUNCTION__);
            urlEntry = nullptr;
        }
    } else {
        urlres_e = urlEntry;
        urlres_e->lock("UrnState::start");
        sc = storeClientListAdd(urlres_e, this);
    }

    reqofs = 0;
    StoreIOBuffer tempBuffer;
    tempBuffer.offset = reqofs;
    tempBuffer.length = URN_REQBUF_SZ;
    tempBuffer.data = reqbuf;
    storeClientCopy(sc, urlres_e,
                    tempBuffer,
                    urnHandleReply,
                    this);
}
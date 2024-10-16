UrnState::~UrnState()
{
    SWALLOW_EXCEPTIONS({
        if (urlres_e) {
            if (sc)
                storeUnregister(sc, urlres_e, this);
            urlres_e->unlock("~UrnState+res");
        }

        if (entry)
            entry->unlock("~UrnState+prime");

        safe_free(urlres);
    });
}
QVectorPath::~QVectorPath()
{
    if (m_hints & ShouldUseCacheHint) {
        CacheEntry *e = m_cache;
        while (e) {
            if (e->data)
                e->cleanup(e->engine, e->data);
            CacheEntry *n = e->next;
            delete e;
            e = n;
        }
    }
}
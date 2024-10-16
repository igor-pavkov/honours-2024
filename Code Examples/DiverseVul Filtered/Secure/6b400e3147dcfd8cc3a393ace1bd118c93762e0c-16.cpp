QVectorPath::CacheEntry *QVectorPath::addCacheData(QPaintEngineEx *engine, void *data,
                                                   qvectorpath_cache_cleanup cleanup) const{
    Q_ASSERT(!lookupCacheData(engine));
    if ((m_hints & IsCachedHint) == 0) {
        m_cache = nullptr;
        m_hints |= IsCachedHint;
    }
    CacheEntry *e = new CacheEntry;
    e->engine = engine;
    e->data = data;
    e->cleanup = cleanup;
    e->next = m_cache;
    m_cache = e;
    return m_cache;
}
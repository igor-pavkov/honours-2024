T3FontCache::~T3FontCache() {
  gfree(cacheData);
  gfree(cacheTags);
}
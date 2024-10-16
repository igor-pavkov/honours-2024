static void build_chunks(MOVTrack *trk)
{
    int i;
    MOVIentry *chunk = &trk->cluster[0];
    uint64_t chunkSize = chunk->size;
    chunk->chunkNum = 1;
    if (trk->chunkCount)
        return;
    trk->chunkCount = 1;
    for (i = 1; i<trk->entry; i++){
        if (chunk->pos + chunkSize == trk->cluster[i].pos &&
            chunkSize + trk->cluster[i].size < (1<<20)){
            chunkSize             += trk->cluster[i].size;
            chunk->samples_in_chunk += trk->cluster[i].entries;
        } else {
            trk->cluster[i].chunkNum = chunk->chunkNum+1;
            chunk=&trk->cluster[i];
            chunkSize = chunk->size;
            trk->chunkCount++;
        }
    }
}
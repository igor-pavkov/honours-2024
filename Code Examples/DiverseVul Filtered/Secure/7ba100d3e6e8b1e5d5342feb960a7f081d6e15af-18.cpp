static int playlist_in_multiple_variants(HLSContext *c, struct playlist *pls)
{
    int variant_count = 0;
    int i, j;

    for (i = 0; i < c->n_variants && variant_count < 2; i++) {
        struct variant *v = c->variants[i];

        for (j = 0; j < v->n_playlists; j++) {
            if (v->playlists[j] == pls) {
                variant_count++;
                break;
            }
        }
    }

    return variant_count >= 2;
}
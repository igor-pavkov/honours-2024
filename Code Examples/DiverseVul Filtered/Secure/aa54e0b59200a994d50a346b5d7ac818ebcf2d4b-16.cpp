static bool init_features(ASS_Shaper *shaper)
{
    shaper->features = calloc(sizeof(hb_feature_t), NUM_FEATURES);
    if (!shaper->features)
        return false;

    shaper->n_features = NUM_FEATURES;
    shaper->features[VERT].tag = HB_TAG('v', 'e', 'r', 't');
    shaper->features[VERT].end = UINT_MAX;
    shaper->features[VKNA].tag = HB_TAG('v', 'k', 'n', 'a');
    shaper->features[VKNA].end = UINT_MAX;
    shaper->features[KERN].tag = HB_TAG('k', 'e', 'r', 'n');
    shaper->features[KERN].end = UINT_MAX;
    shaper->features[LIGA].tag = HB_TAG('l', 'i', 'g', 'a');
    shaper->features[LIGA].end = UINT_MAX;
    shaper->features[CLIG].tag = HB_TAG('c', 'l', 'i', 'g');
    shaper->features[CLIG].end = UINT_MAX;

    return true;
}
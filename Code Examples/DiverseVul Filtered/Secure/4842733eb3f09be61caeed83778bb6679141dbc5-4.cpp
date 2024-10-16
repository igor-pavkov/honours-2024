int cli_hm_have_size(const struct cli_matcher *root, enum CLI_HASH_TYPE type, uint32_t size) {
    return (size && size != 0xffffffff && root && root->hm.sizehashes[type].capacity && cli_htu32_find(&root->hm.sizehashes[type], size));
}
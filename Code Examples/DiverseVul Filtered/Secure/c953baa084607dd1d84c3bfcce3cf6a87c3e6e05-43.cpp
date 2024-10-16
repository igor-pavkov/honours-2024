static void mov_free_encryption_index(MOVEncryptionIndex **index) {
    int i;
    if (!index || !*index) return;
    for (i = 0; i < (*index)->nb_encrypted_samples; i++) {
        av_encryption_info_free((*index)->encrypted_samples[i]);
    }
    av_freep(&(*index)->encrypted_samples);
    av_freep(&(*index)->auxiliary_info_sizes);
    av_freep(&(*index)->auxiliary_offsets);
    av_freep(index);
}
PHPAPI zval *var_tmp_var(php_unserialize_data_t *var_hashx)
{
    var_dtor_entries *var_hash;

    if (!var_hashx || !*var_hashx) {
        return NULL;
    }

    var_hash = (*var_hashx)->last_dtor;
    if (!var_hash || var_hash->used_slots == VAR_ENTRIES_MAX) {
        var_hash = emalloc(sizeof(var_dtor_entries));
        var_hash->used_slots = 0;
        var_hash->next = 0;

        if (!(*var_hashx)->first_dtor) {
            (*var_hashx)->first_dtor = var_hash;
        } else {
            ((var_dtor_entries *) (*var_hashx)->last_dtor)->next = var_hash;
        }

        (*var_hashx)->last_dtor = var_hash;
    }
    ZVAL_UNDEF(&var_hash->data[var_hash->used_slots]);
    return &var_hash->data[var_hash->used_slots++];
}
UpdateDerivedKeymapFields(struct xkb_keymap *keymap)
{
    struct xkb_key *key;
    struct xkb_mod *mod;
    struct xkb_led *led;
    unsigned int i, j;

    /* Find all the interprets for the key and bind them to actions,
     * which will also update the vmodmap. */
    xkb_keys_foreach(key, keymap)
        if (!ApplyInterpsToKey(keymap, key))
            return false;

    /* Update keymap->mods, the virtual -> real mod mapping. */
    xkb_keys_foreach(key, keymap)
        xkb_mods_enumerate(i, mod, &keymap->mods)
            if (key->vmodmap & (1u << i))
                mod->mapping |= key->modmap;

    /* Now update the level masks for all the types to reflect the vmods. */
    for (i = 0; i < keymap->num_types; i++) {
        ComputeEffectiveMask(keymap, &keymap->types[i].mods);

        for (j = 0; j < keymap->types[i].num_entries; j++) {
            ComputeEffectiveMask(keymap, &keymap->types[i].entries[j].mods);
            ComputeEffectiveMask(keymap, &keymap->types[i].entries[j].preserve);
        }
    }

    /* Update action modifiers. */
    xkb_keys_foreach(key, keymap)
        for (i = 0; i < key->num_groups; i++)
            for (j = 0; j < XkbKeyNumLevels(key, i); j++)
                UpdateActionMods(keymap, &key->groups[i].levels[j].action,
                                 key->modmap);

    /* Update vmod -> led maps. */
    xkb_leds_foreach(led, keymap)
        ComputeEffectiveMask(keymap, &led->mods);

    /* Find maximum number of groups out of all keys in the keymap. */
    xkb_keys_foreach(key, keymap)
        keymap->num_groups = MAX(keymap->num_groups, key->num_groups);

    return true;
}
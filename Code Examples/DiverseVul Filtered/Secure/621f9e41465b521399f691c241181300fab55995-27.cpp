static void annotation_get_annotsize(annotate_state_t *state,
                                struct annotate_entry_list *entry)
{
    struct mailbox *mailbox = state->mailbox;
    struct buf value = BUF_INITIALIZER;

    assert(mailbox);

    buf_printf(&value, QUOTA_T_FMT, mailbox->i.quota_annot_used);
    output_entryatt(state, entry->name, "", &value);
    buf_free(&value);
}
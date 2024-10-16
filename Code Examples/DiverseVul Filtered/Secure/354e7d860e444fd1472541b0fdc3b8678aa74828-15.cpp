const char *ct_state_to_string(uint32_t state)
{
    switch (state) {
    case CS_REPLY_DIR:
        return "rpl";
    case CS_TRACKED:
        return "trk";
    case CS_NEW:
        return "new";
    case CS_ESTABLISHED:
        return "est";
    case CS_RELATED:
        return "rel";
    case CS_INVALID:
        return "inv";
    default:
        return NULL;
    }
}
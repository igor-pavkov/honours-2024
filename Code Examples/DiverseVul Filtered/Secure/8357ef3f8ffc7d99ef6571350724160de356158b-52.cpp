static void AppLayerProtoDetectProbingParserAppend(AppLayerProtoDetectProbingParser **head_pp,
                                                   AppLayerProtoDetectProbingParser *new_pp)
{
    SCEnter();

    if (*head_pp == NULL) {
        *head_pp = new_pp;
        goto end;
    }

    AppLayerProtoDetectProbingParser *temp_pp = *head_pp;
    while (temp_pp->next != NULL)
        temp_pp = temp_pp->next;
    temp_pp->next = new_pp;

 end:
    SCReturn;
}
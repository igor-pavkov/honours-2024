static void AppLayerProtoDetectProbingParserElementAppend(AppLayerProtoDetectProbingParserElement **head_pe,
                                                          AppLayerProtoDetectProbingParserElement *new_pe)
{
    SCEnter();

    if (*head_pe == NULL) {
        *head_pe = new_pe;
        goto end;
    }

    if ((*head_pe)->port == 0) {
        if (new_pe->port != 0) {
            new_pe->next = *head_pe;
            *head_pe = new_pe;
        } else {
            AppLayerProtoDetectProbingParserElement *temp_pe = *head_pe;
            while (temp_pe->next != NULL)
                temp_pe = temp_pe->next;
            temp_pe->next = new_pe;
        }
    } else {
        AppLayerProtoDetectProbingParserElement *temp_pe = *head_pe;
        if (new_pe->port == 0) {
            while (temp_pe->next != NULL)
                temp_pe = temp_pe->next;
            temp_pe->next = new_pe;
        } else {
            while (temp_pe->next != NULL && temp_pe->next->port != 0)
                temp_pe = temp_pe->next;
            new_pe->next = temp_pe->next;
            temp_pe->next = new_pe;

        }
    }

 end:
    SCReturn;
}
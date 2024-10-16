static void AppLayerProtoDetectProbingParserPortAppend(AppLayerProtoDetectProbingParserPort **head_port,
                                                       AppLayerProtoDetectProbingParserPort *new_port)
{
    SCEnter();

    if (*head_port == NULL) {
        *head_port = new_port;
        goto end;
    }

    if ((*head_port)->port == 0) {
        new_port->next = *head_port;
        *head_port = new_port;
    } else {
        AppLayerProtoDetectProbingParserPort *temp_port = *head_port;
        while (temp_port->next != NULL && temp_port->next->port != 0) {
            temp_port = temp_port->next;
        }
        new_port->next = temp_port->next;
        temp_port->next = new_port;
    }

 end:
    SCReturn;
}
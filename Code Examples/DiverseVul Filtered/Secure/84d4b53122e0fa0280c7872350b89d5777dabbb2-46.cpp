static void MqttClient_RespList_Remove(MqttClient *client, MqttPendResp *rmResp)
{
    MqttPendResp *tmpResp;

    if (client == NULL)
        return;

#ifdef WOLFMQTT_DEBUG_CLIENT
    PRINTF("PendResp Remove: %p", rmResp);
#endif

    /* Find the response entry */
    for (tmpResp = client->firstPendResp;
         tmpResp != NULL;
         tmpResp = tmpResp->next)
    {
        if (tmpResp == rmResp) {
            break;
        }
    }
    if (tmpResp) {
        /* Fix up the first and last pointers */
        if (client->firstPendResp == tmpResp) {
            client->firstPendResp = tmpResp->next;
        }
        if (client->lastPendResp == tmpResp) {
            client->lastPendResp = tmpResp->prev;
        }

        /* Remove the entry from the list */
        if (tmpResp->next != NULL) {
            tmpResp->next->prev = tmpResp->prev;
        }
        if (tmpResp->prev != NULL) {
            tmpResp->prev->next = tmpResp->next;
        }
    }
#ifdef WOLFMQTT_DEBUG_CLIENT
    else {
        PRINTF("\tPendResp Remove Failed");
    }
#endif
}
void MqttClient_DeInit(MqttClient *client)
{
    if (client != NULL) {
#ifdef WOLFMQTT_MULTITHREAD
        (void)wm_SemFree(&client->lockSend);
        (void)wm_SemFree(&client->lockRecv);
        (void)wm_SemFree(&client->lockClient);
#endif
    }
#ifdef WOLFMQTT_V5
    (void)MqttProps_ShutDown();
#endif
}
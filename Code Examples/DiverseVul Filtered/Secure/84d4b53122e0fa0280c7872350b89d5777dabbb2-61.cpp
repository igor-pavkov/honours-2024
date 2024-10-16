int SN_Client_SetRegisterCallback(MqttClient *client,
        SN_ClientRegisterCb regCb,
        void* ctx)
{
    int rc = MQTT_CODE_SUCCESS;

    if (client == NULL)
        return MQTT_CODE_ERROR_BAD_ARG;

#ifdef WOLFMQTT_MULTITHREAD
    rc = wm_SemLock(&client->lockClient);
    if (rc == 0) {
#endif

        client->reg_cb = regCb;
        client->reg_ctx = ctx;

#ifdef WOLFMQTT_MULTITHREAD
        wm_SemUnlock(&client->lockClient);
    }
#endif

    return rc;
}
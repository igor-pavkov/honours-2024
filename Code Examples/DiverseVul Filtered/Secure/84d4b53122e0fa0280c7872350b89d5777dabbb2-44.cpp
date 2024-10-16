int SN_Client_WaitMessage(MqttClient *client, int timeout_ms)
{
    if (client == NULL)
        return MQTT_CODE_ERROR_BAD_ARG;
    return SN_Client_WaitMessage_ex(client, &client->msgSN, timeout_ms);
}
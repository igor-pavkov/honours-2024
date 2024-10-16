int MqttClient_WaitMessage(MqttClient *client, int timeout_ms)
{
    if (client == NULL)
        return MQTT_CODE_ERROR_BAD_ARG;
    return MqttClient_WaitMessage_ex(client, &client->msg, timeout_ms);
}
const char* MqttClient_ReturnCodeToString(int return_code)
{
    switch(return_code) {
        case MQTT_CODE_SUCCESS:
            return "Success";
        case MQTT_CODE_CONTINUE:
            return "Continue"; /* would block */
        case MQTT_CODE_STDIN_WAKE:
            return "STDIN Wake";
        case MQTT_CODE_PUB_CONTINUE:
            return "Continue calling publish"; /* Chunked publish */
        case MQTT_CODE_ERROR_BAD_ARG:
            return "Error (Bad argument)";
        case MQTT_CODE_ERROR_OUT_OF_BUFFER:
            return "Error (Out of buffer)";
        case MQTT_CODE_ERROR_MALFORMED_DATA:
            return "Error (Malformed Remaining Length)";
        case MQTT_CODE_ERROR_PACKET_TYPE:
            return "Error (Packet Type Mismatch)";
        case MQTT_CODE_ERROR_PACKET_ID:
            return "Error (Packet Id Mismatch)";
        case MQTT_CODE_ERROR_TLS_CONNECT:
            return "Error (TLS Connect)";
        case MQTT_CODE_ERROR_TIMEOUT:
            return "Error (Timeout)";
        case MQTT_CODE_ERROR_NETWORK:
            return "Error (Network)";
        case MQTT_CODE_ERROR_MEMORY:
            return "Error (Memory)";
        case MQTT_CODE_ERROR_STAT:
            return "Error (State)";
        case MQTT_CODE_ERROR_PROPERTY:
            return "Error (Property)";
        case MQTT_CODE_ERROR_SERVER_PROP:
            return "Error (Server Property)";
        case MQTT_CODE_ERROR_CALLBACK:
            return "Error (Error in Callback)";
        case MQTT_CODE_ERROR_SYSTEM:
            return "Error (System resource failed)";
    }
    return "Unknown";
}
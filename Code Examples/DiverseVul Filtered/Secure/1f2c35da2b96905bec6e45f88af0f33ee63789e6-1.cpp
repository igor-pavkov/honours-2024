void auth_policy_parse_response(struct policy_lookup_ctx *context)
{
	enum json_type type;
	const char *value;
	int ret;

	while((ret = json_parse_next(context->parser, &type, &value)) == 1) {
		if (context->parse_state == POLICY_RESULT) {
			if (type != JSON_TYPE_OBJECT_KEY)
				break;
			else if (strcmp(value, "status") == 0)
				context->parse_state = POLICY_RESULT_VALUE_STATUS;
			else if (strcmp(value, "msg") == 0)
				context->parse_state = POLICY_RESULT_VALUE_MESSAGE;
			else break;
		} else if (context->parse_state == POLICY_RESULT_VALUE_STATUS) {
			if (type != JSON_TYPE_NUMBER || str_to_int(value, &(context->result)) != 0)
				break;
			context->parse_state = POLICY_RESULT;
		} else if (context->parse_state == POLICY_RESULT_VALUE_MESSAGE) {
			if (type != JSON_TYPE_STRING)
				break;
			if (*value != '\0')
				context->message = p_strdup(context->pool, value);
			context->parse_state = POLICY_RESULT;
		} else {
			break;
		}
	}

	if (ret == 0 && !context->payload->eof)
		return;

	context->parse_error = TRUE;

	io_remove(&(context->io));

	if (context->payload->stream_errno != 0) {
		auth_request_log_error(context->request, "policy",
			"Error reading policy server result: %s",
			i_stream_get_error(context->payload));
	} else if (ret == 0 && context->payload->eof) {
		auth_request_log_error(context->request, "policy",
			"Policy server result was too short");
	} else if (ret == 1) {
		auth_request_log_error(context->request, "policy",
			"Policy server response was malformed");
	} else {
		const char *error = "unknown";
		if (json_parser_deinit(&(context->parser), &error) != 0)
			auth_request_log_error(context->request, "policy",
				"Policy server response JSON parse error: %s", error);
		else if (context->parse_state == POLICY_RESULT)
			context->parse_error = FALSE;
	}
	i_stream_unref(&(context->payload));

	if (context->parse_error) {
		context->result = (context->set->policy_reject_on_fail ? -1 : 0);
	}

	context->request->policy_refusal = FALSE;

	if (context->result < 0) {
		if (context->message != NULL) {
			/* set message here */
			auth_request_log_debug(context->request, "policy",
				"Policy response %d with message: %s",
				context->result, context->message);
			auth_request_set_field(context->request, "reason", context->message, NULL);
		}
		context->request->policy_refusal = TRUE;
	} else {
		auth_request_log_debug(context->request, "policy",
			"Policy response %d", context->result);
	}

	if (context->request->policy_refusal == TRUE && context->set->verbose == TRUE) {
		auth_request_log_info(context->request, "policy", "Authentication failure due to policy server refusal%s%s",
			(context->message!=NULL?": ":""),
			(context->message!=NULL?context->message:""));
	}

	if (context->callback != NULL) {
		context->callback(context->result, context->callback_context);
	}
};
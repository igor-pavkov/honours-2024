static int oidc_request_post_preserved_restore(request_rec *r,
		const char *original_url) {

	oidc_debug(r, "enter: original_url=%s", original_url);

	const char *method = "postOnLoad";
	const char *script =
			apr_psprintf(r->pool,
					"    <script type=\"text/javascript\">\n"
					"      function %s() {\n"
					"        var mod_auth_openidc_preserve_post_params = JSON.parse(localStorage.getItem('mod_auth_openidc_preserve_post_params'));\n"
					"		 localStorage.removeItem('mod_auth_openidc_preserve_post_params');\n"
					"        for (var key in mod_auth_openidc_preserve_post_params) {\n"
					"          var input = document.createElement(\"input\");\n"
					"          input.name = decodeURIComponent(key);\n"
					"          input.value = decodeURIComponent(mod_auth_openidc_preserve_post_params[key]);\n"
					"          input.type = \"hidden\";\n"
					"          document.forms[0].appendChild(input);\n"
					"        }\n"
					"        document.forms[0].action = '%s';\n"
					"        document.forms[0].submit();\n"
					"      }\n"
					"    </script>\n", method, original_url);

	const char *body = "    <p>Restoring...</p>\n"
			"    <form method=\"post\"></form>\n";

	return oidc_util_html_send(r, "Restoring...", script, method, body,
			DONE);
}
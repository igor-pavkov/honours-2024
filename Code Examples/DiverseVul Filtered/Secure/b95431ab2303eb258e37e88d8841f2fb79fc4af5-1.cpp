static int acl_add(struct ldb_module *module, struct ldb_request *req)
{
	int ret;
	struct ldb_dn *parent;
	struct ldb_context *ldb;
	const struct dsdb_schema *schema;
	const struct dsdb_class *objectclass;
	struct ldb_control *as_system;
	struct ldb_message_element *el;
	unsigned int instanceType = 0;

	if (ldb_dn_is_special(req->op.add.message->dn)) {
		return ldb_next_request(module, req);
	}

	as_system = ldb_request_get_control(req, LDB_CONTROL_AS_SYSTEM_OID);
	if (as_system != NULL) {
		as_system->critical = 0;
	}

	if (dsdb_module_am_system(module) || as_system) {
		return ldb_next_request(module, req);
	}

	ldb = ldb_module_get_ctx(module);

	parent = ldb_dn_get_parent(req, req->op.add.message->dn);
	if (parent == NULL) {
		return ldb_oom(ldb);
	}

	schema = dsdb_get_schema(ldb, req);
	if (!schema) {
		return ldb_operr(ldb);
	}

	objectclass = dsdb_get_structural_oc_from_msg(schema, req->op.add.message);
	if (!objectclass) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "acl: unable to find or validate structural objectClass on %s\n",
				       ldb_dn_get_linearized(req->op.add.message->dn));
		return ldb_module_done(req, NULL, NULL, LDB_ERR_OPERATIONS_ERROR);
	}

	el = ldb_msg_find_element(req->op.add.message, "instanceType");
	if ((el != NULL) && (el->num_values != 1)) {
		ldb_set_errstring(ldb, "acl: the 'instanceType' attribute is single-valued!");
		return LDB_ERR_UNWILLING_TO_PERFORM;
	}

	instanceType = ldb_msg_find_attr_as_uint(req->op.add.message,
						 "instanceType", 0);
	if (instanceType & INSTANCE_TYPE_IS_NC_HEAD) {
		static const char *no_attrs[] = { NULL };
		struct ldb_result *partition_res;
		struct ldb_dn *partitions_dn;

		partitions_dn = samdb_partitions_dn(ldb, req);
		if (!partitions_dn) {
			ldb_set_errstring(ldb, "acl: CN=partitions dn could not be generated!");
			return LDB_ERR_UNWILLING_TO_PERFORM;
		}

		ret = dsdb_module_search(module, req, &partition_res,
					 partitions_dn, LDB_SCOPE_ONELEVEL,
					 no_attrs,
					 DSDB_FLAG_NEXT_MODULE |
					 DSDB_FLAG_AS_SYSTEM |
					 DSDB_SEARCH_ONE_ONLY |
					 DSDB_SEARCH_SHOW_RECYCLED,
					 req,
					 "(&(nCName=%s)(objectClass=crossRef))",
					 ldb_dn_get_linearized(req->op.add.message->dn));

		if (ret == LDB_SUCCESS) {
			/* Check that we can write to the crossRef object MS-ADTS 3.1.1.5.2.8.2 */
			ret = dsdb_module_check_access_on_dn(module, req, partition_res->msgs[0]->dn,
							     SEC_ADS_WRITE_PROP,
							     &objectclass->schemaIDGUID, req);
			if (ret != LDB_SUCCESS) {
				ldb_asprintf_errstring(ldb_module_get_ctx(module),
						       "acl: ACL check failed on crossRef object %s: %s\n",
						       ldb_dn_get_linearized(partition_res->msgs[0]->dn),
						       ldb_errstring(ldb));
				return ret;
			}

			/*
			 * TODO: Remaining checks, like if we are
			 * the naming master etc need to be handled
			 * in the instanceType module
			 */
			return ldb_next_request(module, req);
		}

		/* Check that we can create a crossRef object MS-ADTS 3.1.1.5.2.8.2 */
		ret = dsdb_module_check_access_on_dn(module, req, partitions_dn,
						     SEC_ADS_CREATE_CHILD,
						     &objectclass->schemaIDGUID, req);
		if (ret == LDB_ERR_NO_SUCH_OBJECT &&
		    ldb_request_get_control(req, LDB_CONTROL_RELAX_OID))
		{
			/* Allow provision bootstrap */
			ret = LDB_SUCCESS;
		}
		if (ret != LDB_SUCCESS) {
			ldb_asprintf_errstring(ldb_module_get_ctx(module),
					       "acl: ACL check failed on CN=Partitions crossRef container %s: %s\n",
					       ldb_dn_get_linearized(partitions_dn), ldb_errstring(ldb));
			return ret;
		}

		/*
		 * TODO: Remaining checks, like if we are the naming
		 * master and adding the crossRef object need to be
		 * handled in the instanceType module
		 */
		return ldb_next_request(module, req);
	}

	ret = dsdb_module_check_access_on_dn(module, req, parent,
					     SEC_ADS_CREATE_CHILD,
					     &objectclass->schemaIDGUID, req);
	if (ret != LDB_SUCCESS) {
		ldb_asprintf_errstring(ldb_module_get_ctx(module),
				       "acl: unable to get access to %s\n",
				       ldb_dn_get_linearized(req->op.add.message->dn));
		return ret;
	}
	return ldb_next_request(module, req);
}
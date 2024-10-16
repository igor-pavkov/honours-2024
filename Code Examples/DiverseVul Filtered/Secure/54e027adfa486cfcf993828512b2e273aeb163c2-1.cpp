int registered(struct sip_msg* _m, char* _t, char* _s, char *_c)
{
	str uri, aor;
	urecord_t* r;
	ucontact_t* ptr;
	pv_value_t val;
	str callid;
	int res;

	/* get the AOR */
	if (_s) {
		if (pv_get_spec_value( _m, (pv_spec_p)_s, &val)!=0) {
			LM_ERR("failed to getAOR PV value\n");
			return -1;
		}
		if ( (val.flags&PV_VAL_STR)==0 ) {
			LM_ERR("AOR PV vals is not string\n");
			return -1;
		}
		uri = val.rs;
	} else {
		if (_m->first_line.type!=SIP_REQUEST) {
			LM_ERR("no AOR and called for a reply!");
			return -1;
		}
		if (_m->new_uri.s) uri = _m->new_uri;
		else uri = _m->first_line.u.request.uri;
	}

	if (extract_aor(&uri, &aor,0,0) < 0) {
		LM_ERR("failed to extract address of record\n");
		return -1;
	}

	/* get the callid */
	if (_c) {
		if (pv_get_spec_value( _m, (pv_spec_p)_c, &val)!=0) {
			LM_ERR("failed to get callid PV value\n");
			return -1;
		}
		if ( (val.flags&PV_VAL_STR)==0 ) {
			LM_ERR("callid PV vals is not string\n");
			return -1;
		}
		callid = val.rs;
	} else {
		callid.s = NULL;
		callid.len = 0;
	}

	ul.lock_udomain((udomain_t*)_t, &aor);
	res = ul.get_urecord((udomain_t*)_t, &aor, &r);

	if (res < 0) {
		ul.unlock_udomain((udomain_t*)_t, &aor);
		LM_ERR("failed to query usrloc\n");
		return -1;
	}

	if (res == 0) {
		ptr = r->contacts;
		while (ptr && !VALID_CONTACT(ptr, act_time)) {
			ptr = ptr->next;
		}

		for( ; ptr ; ptr=ptr->next ) {
			if (callid.len==0 || (callid.len==ptr->callid.len &&
			memcmp(callid.s,ptr->callid.s,callid.len)==0 ) ) {
				ul.unlock_udomain((udomain_t*)_t, &aor);
				LM_DBG("'%.*s' found in usrloc\n", aor.len, ZSW(aor.s));
				return 1;
			}
		}
	}

	ul.unlock_udomain((udomain_t*)_t, &aor);
	LM_DBG("'%.*s' not found in usrloc\n", aor.len, ZSW(aor.s));
	return -1;
}
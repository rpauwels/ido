insert into event (id, version, header, summary, location, lat, lon, start, end)
	values (1, 0, 'ja', 'Huwelijksceremonie', 'Gemeentehuis, Markt 1, Temse', 0, 0, "", "");

insert into event_party (event_id, party_id)
	values (1, 1);

insert into party (id, version, uuid, name, email, inviteLevel, invited, opened, remarks, confirmed) 
	values (1, 0, "test", "Hoogheid", "raf@localhost", 0, null, null, "", null);

insert into guest 
	values (1, 0, "Raf", "Pauwels", 0, 1, 1);

insert into guest values (2, 0, "Véronique", "Cordier", 0, 1, 1);

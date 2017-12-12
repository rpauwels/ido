insert into event (id, version, header, summary, location, lat, lon, start, end)
	values (0, 0, 'ja', 'Huwelijksceremonie', 'Gemeentehuis, Markt 1, Temse', 51.124517, 4.214168, '2018-02-03T11:00:00.000', '2018-02-03T12:00:00.000');
insert into event values (1, 0, 'hap', 'Lunch', 'Eventroom, Buisstraat 19, Sint-Amands', 51.056199, 4.211381, '2018-02-03T12:30:00.000', '2018-02-03T13:30:00.000');
insert into event values (2, 0, 'klik', 'Foto''s', 'Park, Sint-Amands', 0, 0, '2018-02-03T12:30:00.000', '2018-02-03T13:30:00.000');
insert into event values (3, 0, 'chin', 'Receptie', 'Eventroom, Buisstraat 19, Sint-Amands', 51.056199, 4.211381, '2018-02-03T18:00:00.000', '2018-02-03T19:30:00.000');
insert into event values (4, 0, 'njam', 'Diner', 'Eventroom, Buisstraat 19, Sint-Amands', 51.056199, 4.211381, '2018-02-03T19:30:00.000', '2018-02-03T22:00:00.000');
insert into event values (5, 0, 'chin', 'Receptie', 'Eventroom, Buisstraat 19, Sint-Amands', 51.056199, 4.211381, '2018-02-03T21:30:00.000', '2018-02-03T22:00:00.000');
insert into event values (6, 0, 'mmm', 'Dessertenbuffet', 'Eventroom, Buisstraat 19, Sint-Amands', 51.056199, 4.211381, '2018-02-03T22:00:00.000', '2018-02-03T23:00:00.000');

insert into party (id, version, uuid, name, email, inviteLevel, invited, opened, remarks, confirmed) 
	values (0, 0, "test", "Hoogheid", "raf@localhost", 2, null, null, "", null);
insert into party (id, version, uuid, name, email, inviteLevel, invited, opened, remarks, confirmed) 
	values (1, 0, lower(hex(randomblob(16))), "echte uuid", "raf@localhost", 1, null, null, "", null);

insert into guest (id, version, first_name, last_name, "order", diet, party_id)
	values (0, 0, "Raf", "Pauwels", 0, 1, 0);
insert into guest values (2, 0, "Véronique", "Cordier", 0, 2, 0);

insert into event_party (event_id, party_id)
	select 0, id 
	from party;
insert into event_party
	select 1, id 
	from party 
	where inviteLevel = 2;
insert into event_party
	select 2, id 
	from party 
	where inviteLevel = 2;
insert into event_party
	select 3, id 
	from party 
	where inviteLevel >= 1;
insert into event_party
	select 4, id 
	from party 
	where inviteLevel >= 1;
insert into event_party
	select 5, id 
	from party 
	where inviteLevel = 0;
insert into event_party
	select 6, id 
	from party 
	where inviteLevel = 0;
	
update party
	set uuid = lower(hex(randomblob(16)));
	
update guest
	set email='raf@localhost'
	where email is not null and email != '';

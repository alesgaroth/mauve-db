create database mauve_test ;
create table mauve_test.color (
				name varchar(255) not null primary key);
use mauve_test ;
create table person (id integer not null primary key auto_increment,
		name varchar(255) not null);
insert into person (id, name)
		values(1, 'mauveuser');
insert into person set name = 'dilbert';
drop table person;
drop database mauve_test ;

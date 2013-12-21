create database mauve_test ;
use mauve_test ;
create table person (id integer not null primary key auto_increment,
		name varchar(255) not null);
insert into person (id, id) values(7, 8);
drop database mauve_test ;

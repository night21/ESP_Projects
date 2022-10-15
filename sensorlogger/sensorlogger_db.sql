use data_repository;
DROP TABLE sensor_data;
CREATE TABLE sensor_data (
	id INT NOT NULL AUTO_INCREMENT,
	type VARCHAR(15) NOT NULL,
	time DATETIME NOT NULL,
	value FLOAT NOT NULL,
	PRIMARY KEY(id)
);

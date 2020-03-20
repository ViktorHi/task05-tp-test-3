PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE Autopark_cars (
    car_number text NOT NULL CONSTRAINT Autopark_cars_pk PRIMARY KEY,
    brand text NOT NULL,
    start_mileage real NOT NULL,
    carrying real NOT NULL
);
INSERT INTO Autopark_cars VALUES('0756KT','man',500.0,10.0);
INSERT INTO Autopark_cars VALUES('1234KT','man',0.0,15.0);
INSERT INTO Autopark_cars VALUES('5608PB','scania',432.49999999999999998,14.5);
INSERT INTO Autopark_cars VALUES('4530TE','scania',1200.3399999999999181,12.0);
INSERT INTO Autopark_cars VALUES('3456KC','man',0.0,14.699999999999999289);
INSERT INTO Autopark_cars VALUES('5609PE','volvo',124.5,8.0);
INSERT INTO Autopark_cars VALUES('5608TH','volvo',0.0,9.5);
INSERT INTO Autopark_cars VALUES('6102CT','faw',3099.9999999999999999,35.0);
INSERT INTO Autopark_cars VALUES('4506EH','faw',2599.9999999999999999,20.0);
INSERT INTO Autopark_cars VALUES('6103TE','volvo',1200.0,7.5);
CREATE TABLE Autopark_completed_orders (
    id integer NOT NULL CONSTRAINT Autopark_completed_orders_pk PRIMARY KEY,
    driver_number integer NOT NULL,
    car_number text NOT NULL,
    mileage real NOT NULL,
    weight real NOT NULL,
    cost real NOT NULL,
    date text NOT NULL,
    CONSTRAINT Autopark_completed_orders_Autopark_drivers FOREIGN KEY (driver_number)
    REFERENCES Autopark_drivers (number),
    CONSTRAINT Autopark_completed_orders_Autopark_cars FOREIGN KEY (car_number)
    REFERENCES Autopark_cars (car_number),
    CONSTRAINT Autopark_completed_orders_Autopark_users FOREIGN KEY (driver_number)
    REFERENCES Autopark_users (id)
);
INSERT INTO Autopark_completed_orders VALUES(1,3,'1234KT',120.0,6.0,200.0,'04-01-2020');
INSERT INTO Autopark_completed_orders VALUES(2,4,'6102CT',30.0,25.0,600.0,'12-12-2019');
INSERT INTO Autopark_completed_orders VALUES(3,1,'6103TE',60.0,4.5,150.0,'10-09-2019');
INSERT INTO Autopark_completed_orders VALUES(4,2,'0756KT',80.0,7.5,250.0,'10-10-2019');
INSERT INTO Autopark_completed_orders VALUES(5,6,'4506EH',70.0,7.5,250.0,'10-01-2020');
INSERT INTO Autopark_completed_orders VALUES(6,7,'5608PB',110.0,12.5,450.0,'12-01-2020');
INSERT INTO Autopark_completed_orders VALUES(7,3,'4506EH',80.0,19.5,1000.0,'12-02-2020');
INSERT INTO Autopark_completed_orders VALUES(8,9,'3456KC',190.0,13.999999999999999999,800.0,'12-03-2020');
INSERT INTO Autopark_completed_orders VALUES(9,8,'3456KC',139.99999999999999999,13.999999999999999999,750.0,'18-03-2020');
INSERT INTO Autopark_completed_orders VALUES(10,10,'5608TH',100.0,8.5,650.0,'08-07-2019');
INSERT INTO Autopark_completed_orders VALUES(11,5,'5609PE',120.0,7.5,600.0,'28-07-2019');
CREATE TABLE Autopark_config (
    profit_percentage integer NOT NULL
);
INSERT INTO Autopark_config VALUES(20);
CREATE TABLE Autopark_drivers (
    number integer NOT NULL CONSTRAINT Autopark_drivers_pk PRIMARY KEY,
    last_name text NOT NULL,
    category text NOT NULL,
    experience integer NOT NULL,
    address text NOT NULL,
    year_of_birth integer NOT NULL
);
INSERT INTO Autopark_drivers VALUES(1,'ivanov','A',10,'MInsk,Surganova 4-103',1976);
INSERT INTO Autopark_drivers VALUES(2,'smirnov','A',9,'Minsk, Dimitrova 9-64',1984);
INSERT INTO Autopark_drivers VALUES(3,'petrov','B',8,'Minsk, Dzerjinskogo, 12-41',1990);
INSERT INTO Autopark_drivers VALUES(4,'egorov','C',3,'Minsk, Sviridova 5-27',1985);
INSERT INTO Autopark_drivers VALUES(5,'ivanov','A',15,'Minsk, Nezavisimosti 56-121',1975);
INSERT INTO Autopark_drivers VALUES(6,'armanov','B',5,'Fanipol, Severnaya 4-9',1995);
INSERT INTO Autopark_drivers VALUES(7,'komarov','D',2,'Fanipol, Oktyabrskaya 35-51',1992);
INSERT INTO Autopark_drivers VALUES(8,'perishkin','D',2,'Minsk, Sverdlova 24-256',1990);
INSERT INTO Autopark_drivers VALUES(9,'patrikov','C',4,'Fanipol, Krasnikova 12-41',1986);
INSERT INTO Autopark_drivers VALUES(10,'reminov','A',14,'Minsk, Stavropolskaya 21-45',1974);
CREATE TABLE Autopark_users (
    id integer NOT NULL CONSTRAINT Autopark_users_pk PRIMARY KEY,
    password text NOT NULL,
    is_admin integer NOT NULL
);
INSERT INTO Autopark_users VALUES(1,'qwer',0);
INSERT INTO Autopark_users VALUES(2,'asdf',0);
INSERT INTO Autopark_users VALUES(3,'zxcv',0);
INSERT INTO Autopark_users VALUES(4,'tyui',0);
INSERT INTO Autopark_users VALUES(5,'ghjk',0);
INSERT INTO Autopark_users VALUES(6,'rewq',0);
INSERT INTO Autopark_users VALUES(7,'fdsa',0);
INSERT INTO Autopark_users VALUES(8,'vcxz',0);
INSERT INTO Autopark_users VALUES(9,'iuyt',0);
INSERT INTO Autopark_users VALUES(10,'kjhg',0);
INSERT INTO Autopark_users VALUES(11,'admin',0);
COMMIT;

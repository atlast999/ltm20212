CREATE TABLE users (
    id INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    credential VARCHAR(100) NOT NULL,
    PRIMARY KEY (id)
);

CREATE TABLE events (
    id INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    description VARCHAR(255),
    time VARCHAR(100) NOT NULL,
    location VARCHAR(100) NOT NULL,
    owner INT NOT NULL,
    PRIMARY KEY (id)
);

CREATE TABLE requests (
    id INT NOT NULL AUTO_INCREMENT,
    eventId INT NOT NULL,
    owner INT NOT NULL,
    target INT NOT NULL,
    type INT NOT NULL,
    status INT NOT NULL,
    PRIMARY KEY (id)
);

INSERT INTO users(name, credential) 
VALUES 
('admin', 'admin');

INSERT INTO events(name, description, time, location, owner)
VALUES
('event of admin1', 'a description', 'tonight', 'my place', 2);
('event1', 'a description', 'tonight', 'my place', 1);

INSERT INTO requests(eventId, owner, target, type, status)
VALUES 
(1, 2, 1, 1, 0),
(2, 2, 1, 2, 0);

SELECT requests.id, requests.type, events.name AS eventName, targetUser.name AS targetUser
    FROM requests, events, users AS targetUser
    WHERE 
        requests.status = 0 and
        requests.owner = 2 and
        events.id = requests.eventId and 
        targetUser.id = requests.target
        ;
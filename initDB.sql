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

CREATE TABLE memberships (
    eventId INT NOT NULL,
    userId INT NOT NULL,
    PRIMARY KEY (eventId, userId)
)

CREATE TABLE requests (
    id INT NOT NULL AUTO_INCREMENT,
    eventId INT NOT NULL,
    owner INT NOT NULL,
    target INT NOT NULL,
    type INT NOT NULL,
    status INT NOT NULL,
    PRIMARY KEY (id)
);

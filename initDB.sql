CREATE TABLE users (
    id INT NOT NULL AUTO_INCREMENT,
    name VARCHAR(100) NOT NULL,
    credential VARCHAR(100) NOT NULL,
    PRIMARY KEY (id)
);

INSERT INTO users(name, credential) 
VALUES 
('admin', 'admin');

SELECT * FROM users
WHERE 
    users.name = 'admin';
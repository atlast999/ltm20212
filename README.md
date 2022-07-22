Compile command: g++ ./Server/Server.cpp -I ./include/

Open design file by https://www.draw.io

Add ./lib/libmysql.dll into executable folder (which contains .exe files)

Setup Mysql via docker: docker compose up -d

Connect Mysql bash: docker exec -it mysql bash -l

Initialize db tables: run initDB.sql

ssh -i "mysql.pem" ubuntu@ec2-35-77-223-104.ap-northeast-1.compute.amazonaws.com

scp -i "mysql.pem" ./docker-compose.yml ubuntu@ec2-35-77-223-104.ap-northeast-1.compute.amazonaws.com:/home/ubuntu/mysql/docker-compose.yml
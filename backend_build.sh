#!/bin/bash

export DISCOVERY_SERVICE_PORT=8761
export DISCOVERY_SERVICE_HOSTNAME=localhost

export PAGODA_JWT_SECRET_KEY=test_key
export GATEWAY_SERVICE_PORT=8080

export PAGODA_JWT_VALIDITY=86400
export AUTHORIZATION_PORT=8081

export CONTAINERS_PORT=8082

export EMAIL_SENDER_ADDRESS=noreply@pagoda.ai
export EMAIL_SENDER_PASSWORD=password
export EMAIL_PORT=8083

export PROJECTS_PORT=8084

cd ./backend/services/common
mvn clean install
cd ../discovery
mvn clean install
cd ../gateway
mvn clean install
cd ../authorization
mvn clean install
cd ../containers
mvn clean install
cd ../email
mvn clean install
cd ../projects
mvn clean install
cd ../../..

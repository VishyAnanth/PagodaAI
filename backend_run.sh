#!/bin/bash

cd ./backend/services/discovery
osascript -e 'tell app "Terminal" to do script "cd '$(pwd)' && export DISCOVERY_SERVICE_PORT=8761 && export DISCOVERY_SERVICE_HOSTNAME=localhost && mvn spring-boot:run"'
cd ../gateway
osascript -e 'tell app "Terminal" to do script "cd '$(pwd)' && export DISCOVERY_SERVICE_PORT=8761 && export DISCOVERY_SERVICE_HOSTNAME=localhost && export PAGODA_JWT_SECRET_KEY=test_key && export GATEWAY_SERVICE_PORT=8080 && mvn spring-boot:run"'
cd ../authorization
osascript -e 'tell app "Terminal" to do script "cd '$(pwd)' && export DISCOVERY_SERVICE_PORT=8761 && export DISCOVERY_SERVICE_HOSTNAME=localhost && export PAGODA_JWT_SECRET_KEY=test_key && export PAGODA_JWT_VALIDITY=86400 && export AUTHORIZATION_PORT=8081 && mvn spring-boot:run"'
cd ../containers
osascript -e 'tell app "Terminal" to do script "cd '$(pwd)' && export DISCOVERY_SERVICE_PORT=8761 && export DISCOVERY_SERVICE_HOSTNAME=localhost && export CONTAINERS_PORT=8082 && mvn spring-boot:run"'
cd ../email
osascript -e 'tell app "Terminal" to do script "cd '$(pwd)' && export DISCOVERY_SERVICE_PORT=8761 && export DISCOVERY_SERVICE_HOSTNAME=localhost && export EMAIL_SENDER_ADDRESS=noreply@pagoda.ai && export EMAIL_SENDER_PASSWORD=password && export EMAIL_PORT=8083 && mvn spring-boot:run"'
cd ../projects
osascript -e 'tell app "Terminal" to do script "cd '$(pwd)' && export DISCOVERY_SERVICE_PORT=8761 && export DISCOVERY_SERVICE_HOSTNAME=localhost && export PROJECTS_PORT=8084 && mvn spring-boot:run"'

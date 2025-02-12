#!/bin/bash
export GAME_DB_URL=postgres://postgres:Mys3Cr3t@192.168.1.200:30432/test_db
#export GAME_DB_URL=postgres://postgres:Mys3Cr3t@localhost:30432/test_db
#./build/bin/game_server --config-file ./data/config.json --www-root static/ --tick-period 100 --randomize-spawn-points
./build/bin/game_server --config-file ./data/config.json --www-root static/ --tick-period 25 
#./build/bin/game_server --config-file ./data/config.json --www-root static/

# psql postgres://postgres:Mys3Cr3t@localhost:30432/test_db
# sudo docker run -d --name postgres-container -p 30432:5432 -e TZ=UTC -e POSTGRES_PASSWORD=Mys3Cr3t ubuntu/postgres:14-22.04_beta

#  sudo docker run --rm -it -p 8080:8080 --env "GAME_DB_URL=postgres://postgres:Mys3Cr3t@192.168.1.200:30432/test_db" game_server
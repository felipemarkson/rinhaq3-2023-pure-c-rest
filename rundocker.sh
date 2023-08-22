set -e
cd src
rm -f *.o
rm -f main
cd db
rm -rf *.sqlite
cd ..
cd ..

sudo docker build . -t rinha && \
sudo docker compose up --remove-orphans 
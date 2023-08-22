set -e
cd src
rm -f *.o
rm -f main
cd ..

sudo docker build . -t rinha && \
sudo docker compose up --remove-orphans 
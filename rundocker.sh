set -e
cd src
rm -f *.o
rm -f main
cd db
rm -rf *.sqlite
cd ..
cd ..

sudo docker build -t rinhaq3-2023-pure-c-rest -t fmarkson/rinhaq3-2023-pure-c-rest .
sudo docker push fmarkson/rinhaq3-2023-pure-c-rest
# sudo docker compose up --remove-orphans 
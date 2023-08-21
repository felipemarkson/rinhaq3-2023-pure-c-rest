set -e
cd src
rm *.o
rm main
cd ..

sudo docker build . -t rinha && \
sudo docker compose up
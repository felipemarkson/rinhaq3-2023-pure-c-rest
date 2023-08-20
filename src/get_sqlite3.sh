set -xe

SQLITE_VERSION=3420000
SQLITE_PKG_NAME=sqlite-amalgamation-$SQLITE_VERSION
wget https://www.sqlite.org/2023/$SQLITE_PKG_NAME.zip
unzip $SQLITE_PKG_NAME.zip
mv $SQLITE_PKG_NAME sqlite3
rm $SQLITE_PKG_NAME.zip
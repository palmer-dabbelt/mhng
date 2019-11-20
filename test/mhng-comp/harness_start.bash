set -ex

tempdir=`mktemp -d -t mhng-test.XXXXXXXXXX`
trap "rm -rf $tempdir" EXIT
cd $tempdir

mkdir maildir
export MHNG_MAILDIR="$(pwd)/maildir"
export MHNG_NO_DAEMON="true"
export MHNG_EDITOR="$(pwd)/mhng-editor"
export MHNG_MAILRC="$(pwd)/mailrc"

cat >init.sql <<EOF
CREATE TABLE MH__current (folder STRING NOT NULL CHECK(folder != ''),
                          seq INTEGER NOT NULL CHECK(seq > 0),
                          cur INTEGER CHECK(cur > 0 AND cur <= 1),
                          uid_validity INTEGER CHECK(uid_validity >= 0),
                          UNIQUE(folder),
                          UNIQUE(cur)
       );
INSERT INTO MH__current (folder, seq, cur) VALUES ("inbox", 1, 1);
INSERT INTO MH__current (folder, seq) VALUES ("drafts", 1);
INSERT INTO MH__current (folder, seq) VALUES ("rss", 1);
INSERT INTO MH__current (folder, seq) VALUES ("promo", 1);
INSERT INTO MH__current (folder, seq) VALUES ("lists", 1);
INSERT INTO MH__current (folder, seq) VALUES ("sent", 1);
INSERT INTO MH__current (folder, seq) VALUES ("trash", 1);

CREATE TABLE MH__messages (uid INTEGER PRIMARY KEY CHECK(uid > 0),
                           seq INTEGER NOT NULL CHECK(seq > 0),
                           folder STRING NOT NULL CHECK(folder != ''),
                           unread INTEGER NOT NULL CHECK(unread >= 0 AND unread <= 2),
                           subject STRING NOT NULL,
                           date INTEGER NOT NULL CHECK(date > 0),
                           fadr STRING NOT NULL,
                           tadr STRING NOT NULL,
                           UNIQUE(uid),
                           UNIQUE(folder, seq)
       );

CREATE TABLE MH__nextid (single INTEGER NOT NULL CHECK(single > 0 AND single < 2),
                         uid INTEGER NOT NULL,
                         UNIQUE(uid),
                         UNIQUE(single)
       );
INSERT INTO MH__nextid (single, uid) VALUES (1, 2);

CREATE TABLE IMAP__messages (folder STRING NOT NULL,
                             uid INTEGER NOT NULL,
                             mhid INTEGER NOT NULL,
                             account STRING NOT NULL,
                             purge INTEGER CHECK(purge >= 0 AND purge <= 1),
                             UNIQUE(folder, uid),
                             UNIQUE(mhid)
       );
EOF
sqlite3 $MHNG_MAILDIR/metadata.sqlite3 < init.sql

mkdir -p $MHNG_MAILDIR/tmp
mkdir -p $MHNG_MAILDIR/mail/inbox
mkdir -p $MHNG_MAILDIR/mail/drafts
mkdir -p $MHNG_MAILDIR/mail/rss
mkdir -p $MHNG_MAILDIR/mail/promo
mkdir -p $MHNG_MAILDIR/mail/lists
mkdir -p $MHNG_MAILDIR/mail/sent
mkdir -p $MHNG_MAILDIR/mail/trash

touch $MHNG_MAILRC

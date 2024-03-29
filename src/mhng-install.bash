# Check to see if there's an existing MHng directory, in which case we
# can't really do anything without removing it.
if test -d $HOME/.mhng
then
    echo "$HOME/.mhng exists"
    echo "Refusing to install over existing directory"
    exit 1
fi

# Create the base MHng directory structure
mkdir -p $HOME/.mhng/tmp
mkdir -p $HOME/.mhng/mail/inbox
mkdir -p $HOME/.mhng/mail/drafts
mkdir -p $HOME/.mhng/mail/rss
mkdir -p $HOME/.mhng/mail/promo
mkdir -p $HOME/.mhng/mail/lists
mkdir -p $HOME/.mhng/mail/linux
mkdir -p $HOME/.mhng/mail/berkeley
mkdir -p $HOME/.mhng/mail/upstream

# Initialize the SQLite database that's necessary to make anything
# work with some default options.  Doing this greatly simplifies the
# MHng implementation, as I don't have to have a bunch of create-table
# commands all over the place...
cat > $HOME/.mhng/init.sql <<EOF
CREATE TABLE MH__current (folder STRING NOT NULL CHECK(folder != ''),
                          seq INTEGER NOT NULL CHECK(seq > 0),
                          cur INTEGER CHECK(cur > 0 AND cur <= 1),
                          UNIQUE(folder),
                          UNIQUE(cur)
       );
INSERT INTO MH__current (folder, seq, cur) VALUES ("inbox", 1, 1);
INSERT INTO MH__current (folder, seq) VALUES ("drafts", 1);
INSERT INTO MH__current (folder, seq) VALUES ("rss", 1);
INSERT INTO MH__current (folder, seq) VALUES ("promo", 1);
INSERT INTO MH__current (folder, seq) VALUES ("lists", 1);
INSERT INTO MH__current (folder, seq) VALUES ("linux", 1);
INSERT INTO MH__current (folder, seq) VALUES ("berkeley", 1);
INSERT INTO MH__current (folder, seq) VALUES ("upstream", 1);
INSERT INTO MH__current (folder, seq) VALUES ("riscv", 1);

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
                             purge INTEGER CHECK(purge >= 0 AND purge <= 1),
                             account STRING NOT NULL,
                             UNIQUE(folder, uid),
                             UNIQUE(mhid)
       );

CREATE TABLE MH__accounts (name STRING NOT NULL,
			   auth STRING NOT NULL,
			   UNIQUE(name)
       );

CREATE TABLE MH__oauth2cred(name STRING NOT NULL,
                            client_id STRING NOT NULL,
                            access_token STRING NOT NULL,
                            refresh_token STRING NOT NULL,
			    access_token_expires DATE NOT NULL,
			    UNIQUE(name)
       );

CREATE TABLE MH__userpass(name STRING NOT NULL,
			  pass STRING NOT NULL,
			  UNIQUE(name)
       );

CREATE TABLE IMAP__uidvalidity (account STRING NOT NULL,
                                folder STRING NOT NULL,
                                uidvalidity INTEGER NOT NULL,
                                UNIQUE(folder, account));
EOF

sqlite3 $HOME/.mhng/metadata.sqlite3 < $HOME/.mhng/init.sql

echo "An empty MHng mailbox has been created at $HOME/.mhng."
echo "You can now use 'mhng-add-account' to create a new account."

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

# Initialize the SQLite database that's necessary to make anything
# work with some default options.  Doing this greatly simplifies the
# MHng implementation, as I don't have to have a bunch of create-table
# commands all over the place...
cat > $HOME/.mhng/init.sql <<EOF
CREATE TABLE MH__default (folder STRING CHECK(folder != '')
       );
INSERT INTO MH__default (folder) VALUES ("inbox");

CREATE TABLE MH__current (folder STRING CHECK(folder != ''),
                          seq INTEGER CHECK(seq > 0),
                          UNIQUE(folder)
       );
INSERT INTO MH__current (folder, seq) VALUES ("inbox", 1);
INSERT INTO MH__current (folder, seq) VALUES ("drafts", 1);

CREATE TABLE MH__messages (uid INTEGER PRIMARY KEY CHECK(uid > 0),
                           seq INTEGER CHECK(seq > 0),
                           folder STRING CHECK(folder != ''),
                           unread INTEGER CHECK(unread >= 0 AND unread <= 1),
                           subject STRING,
                           date INTEGER CHECK(date > 0),
                           fadr STRING,
                           tadr STRING,
                           UNIQUE(uid),
                           UNIQUE(folder, seq)
       );

CREATE TABLE IMAP__messages (folder STRING,
                             uid INTEGER,
                             mhid INTEGER,
                             purge INTEGER CHECK(purge >= 0 AND purge <= 1),
                             UNIQUE(folder, uid),
                             UNIQUE(mhid)
       );
EOF

sqlite3 $HOME/.mhng/metadata.sqlite3 < $HOME/.mhng/init.sql
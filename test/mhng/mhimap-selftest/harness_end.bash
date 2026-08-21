diff -u out.gold out.test

# Only some of these care what ends up on stderr.
if [ -f err.gold ]
then
    diff -u err.gold err.test
fi

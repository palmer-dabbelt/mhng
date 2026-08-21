set -ex

tempdir=`mktemp -d -t mhng-test.XXXXXXXXXX`
trap "rm -rf $tempdir" EXIT
cd $tempdir

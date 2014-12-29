#!/bin/sh
WCALC="$1"
TESTFILE="$2"
exec "$WCALC" --defaults --ints < "$TESTFILE" > /dev/null

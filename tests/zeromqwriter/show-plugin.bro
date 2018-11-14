# @TEST-EXEC: bro -NN NCSA::ZeroMQWriter |sed -e 's/version.*)/version)/g' >output
# @TEST-EXEC: btest-diff output

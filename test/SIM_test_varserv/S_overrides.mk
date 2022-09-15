
TRICK_CFLAGS += -I./models
TRICK_CXXFLAGS += -I./models

all: test_client
clean: clean_test_client

TEST_CLIENT_LIBS := -lpthread -lgtest -lgtest_main

test_client: models/test_client/test_client.cpp 
	cd models/test_client; $(TRICK_CXX) test_client.cpp -o test_client $(TEST_CLIENT_LIBS)

clean_test_client:
	rm -f models/test_client/test_client
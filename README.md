# weather-alerts

./b2 --with-program_options runtime-link=static link=static address-model=64

g++ -Wall -std=c++17 -o weather-alerts -I../boost_1_83_0 weather-alerts.cpp -lcurl -lpthread -L../boost_1_83_0/stage/lib -lboost_program_options

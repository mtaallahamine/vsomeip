#!/bin/bash
# Copyright (C) 2015-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Purpose: This script is needed to start the client and service with
# one command. This is necessary as ctest - which is used to run the
# tests - isn't able to start two binaries for one testcase. Therefore
# the testcase simply executes this script. This script then runs client
# and service and checks that both exit sucessfully.

FAIL=0

# NOW WITHOUT JSON
echo "----------------------------------------------"
echo "----------------------------------------------"
echo "       Run test with auto configuration       "
echo "----------------------------------------------"
echo "----------------------------------------------"

export VSOMEIP_CONFIGURATION=restart_routing_test_autoconfig.json

../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!

sleep 2

# Start the service
export VSOMEIP_APPLICATION_NAME=restart_routing_test_service
./restart_routing_test_service &
SERIVCE_PID=$!

# Start the client1
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client1
./restart_routing_test_client &
CLIENT1_PID=$!

# Start the client2
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client2
./restart_routing_test_client &
CLIENT2_PID=$!

# Start the client3
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client3
./restart_routing_test_client &
CLIENT3_PID=$!

# Start the client4
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client4
./restart_routing_test_client &
CLIENT4_PID=$!

sleep 2
echo "----------------------------------------------"
echo "       let routingmanagerd crash (kill -9)           "
echo "----------------------------------------------"
kill -9 $DAEMON_PID
echo "----------------------------------------------"
echo "             restart routingmanagerd                 "
echo "----------------------------------------------"
sleep 2

../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!

wait $SERIVCE_PID || ((FAIL+=1))
wait $CLIENT1_PID || ((FAIL+=1))
wait $CLIENT2_PID || ((FAIL+=1))
wait $CLIENT3_PID || ((FAIL+=1))
wait $CLIENT4_PID || ((FAIL+=1))

kill $DAEMON_PID
wait $DAEMON_PID

# Check if client and server both exited sucessfully and the service didnt't
# have any open tcp/udp sockets
if [ $FAIL -eq 0 ]
then
    echo "Test Succeeded"
else
    exit 1
fi

# NOW WITHOUT VSOMEIPD
echo "----------------------------------------------"
echo "----------------------------------------------"
echo "  Run test with auto configuration no deamon  "
echo "----------------------------------------------"
echo "----------------------------------------------"

sleep 2

# Start the service
export VSOMEIP_APPLICATION_NAME=restart_routing_test_service
./restart_routing_test_service &
SERIVCE_PID=$!

# Start the client1
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client1
./restart_routing_test_client &
CLIENT1_PID=$!

# Start the client2
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client2
./restart_routing_test_client &
CLIENT2_PID=$!

# Start the client3
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client3
./restart_routing_test_client &
CLIENT3_PID=$!

# Start the client4
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client4
./restart_routing_test_client &
CLIENT4_PID=$!

sleep 2
echo "----------------------------------------------"
echo "   let service (routing) crash (kill -9)      "
echo "----------------------------------------------"
kill -9 $SERIVCE_PID
echo "----------------------------------------------"
echo "          restart service (routing)           "
echo "----------------------------------------------"
sleep 2

# Start the service
export VSOMEIP_APPLICATION_NAME=restart_routing_test_service
./restart_routing_test_service &
SERIVCE_PID=$!

wait $SERIVCE_PID || ((FAIL+=1))
wait $CLIENT1_PID || ((FAIL+=1))
wait $CLIENT2_PID || ((FAIL+=1))
wait $CLIENT3_PID || ((FAIL+=1))
wait $CLIENT4_PID || ((FAIL+=1))

# Check if client and server both exited sucessfully and the service didnt't
# have any open tcp/udp sockets
if [ $FAIL -eq 0 ]
then
    echo "Test Succeeded"
else
    exit 1
fi


echo "----------------------------------------------"
echo "----------------------------------------------"
echo "       Run test with json configuration       "
echo "----------------------------------------------"
echo "----------------------------------------------"

sleep 2

export VSOMEIP_CONFIGURATION=restart_routing_test_service.json
../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!

# Start the service
export VSOMEIP_APPLICATION_NAME=restart_routing_test_service
export VSOMEIP_CONFIGURATION=restart_routing_test_service.json
./restart_routing_test_service &
SERIVCE_PID=$!

# Start the client1
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client1
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT1_PID=$!

# Start the client2
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client2
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT2_PID=$!

# Start the client3
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client3
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT3_PID=$!

# Start the client4
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client4
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT4_PID=$!

sleep 2
echo "----------------------------------------------"
echo "       let routingmanagerd crash (kill -9)           "
echo "----------------------------------------------"
kill -9 $DAEMON_PID
echo "----------------------------------------------"
echo "            restart routingmanagerd                  "
echo "----------------------------------------------"
sleep 2

export VSOMEIP_CONFIGURATION=restart_routing_test_service.json
../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!

wait $SERIVCE_PID || ((FAIL+=1))
wait $CLIENT1_PID || ((FAIL+=1))
wait $CLIENT2_PID || ((FAIL+=1))
wait $CLIENT3_PID || ((FAIL+=1))
wait $CLIENT4_PID || ((FAIL+=1))

kill $DAEMON_PID
wait $DAEMON_PID

# Check if client and server both exited sucessfully and the service didnt't
# have any open tcp/udp sockets
if [ $FAIL -eq 0 ]
then
    echo "Test Succeeded"
else
    exit 1
fi

echo "----------------------------------------------"
echo "----------------------------------------------"
echo "Run test with json configuration + kill service"
echo "----------------------------------------------"
echo "----------------------------------------------"

sleep 2

export VSOMEIP_CONFIGURATION=restart_routing_test_service.json
../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!

# Start the service
export VSOMEIP_APPLICATION_NAME=restart_routing_test_service
export VSOMEIP_CONFIGURATION=restart_routing_test_service.json
./restart_routing_test_service &
SERIVCE_PID=$!

# Start the client1
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client1
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT1_PID=$!

# Start the client2
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client2
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT2_PID=$!

# Start the client3
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client3
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT3_PID=$!

# Start the client4
export VSOMEIP_APPLICATION_NAME=restart_routing_test_client4
export VSOMEIP_CONFIGURATION=restart_routing_test_client.json
./restart_routing_test_client &
CLIENT4_PID=$!

sleep 2
echo "----------------------------------------------"
echo "       let routingmanagerd crash (kill -9)           "
echo "----------------------------------------------"
kill -9 $DAEMON_PID
sleep 1
echo "----------------------------------------------"
echo "       let service crash (kill -9)            "
echo "----------------------------------------------"
kill -9 $SERIVCE_PID
echo "----------------------------------------------"
echo "                restart routingmanagerd              "
echo "----------------------------------------------"
sleep 2

export VSOMEIP_CONFIGURATION=restart_routing_test_service.json
../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!

echo "----------------------------------------------"
echo "                restart service               "
echo "----------------------------------------------"
sleep 1

# Start the service again
export VSOMEIP_APPLICATION_NAME=restart_routing_test_service
export VSOMEIP_CONFIGURATION=restart_routing_test_service.json
./restart_routing_test_service &
SERIVCE_PID=$!

wait $SERIVCE_PID || ((FAIL+=1))
wait $CLIENT1_PID || ((FAIL+=1))
wait $CLIENT2_PID || ((FAIL+=1))
wait $CLIENT3_PID || ((FAIL+=1))
wait $CLIENT4_PID || ((FAIL+=1))

kill $DAEMON_PID
wait $DAEMON_PID

# Check if client and server both exited sucessfully and the service didnt't
# have any open tcp/udp sockets
if [ $FAIL -eq 0 ]
then
    exit 0
else
    exit 1
fi

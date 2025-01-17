#!/bin/bash
# Copyright (C) 2015-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

FAIL=0

export VSOMEIP_CONFIGURATION=application_test_no_dispatch_threads.json
./application_test

if [ $? -ne 0 ]
then
    ((FAIL+=1))
fi

export VSOMEIP_CONFIGURATION=application_test.json
./application_test

if [ $? -ne 0 ]
then
    ((FAIL+=1))
fi

cat <<End-of-message
*******************************************************************************
*******************************************************************************
** Now running same tests with routingmanagerd
*******************************************************************************
*******************************************************************************
End-of-message
export VSOMEIP_CONFIGURATION=application_test_no_dispatch_threads_daemon.json
../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!
./application_test
if [ $? -ne 0 ]
then
    ((FAIL+=1))
fi

kill $DAEMON_PID
wait $DAEMON_PID

export VSOMEIP_CONFIGURATION=application_test_daemon.json
../examples/routingmanagerd/./routingmanagerd &
DAEMON_PID=$!
./application_test
if [ $? -ne 0 ]
then
    ((FAIL+=1))
fi

kill $DAEMON_PID

# Check if both exited successfully
if [ $FAIL -eq 0 ]
then
    exit 0
else
    exit 1
fi

#!/bin/bash
# Copyright (C) 2015-2017 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Purpose: This script is needed to start the client and service with
# one command. This is necessary as ctest - which is used to run the
# tests - isn't able to start two binaries for one testcase. Therefore
# the testcase simply executes this script. This script then runs client
# and service and checks that both exit successfully.

if [[ $# -gt 0 && $1 != "RANDOM" && $1 != "LIMITED" && $1 != "QUEUELIMITEDGENERAL" ]]
then
    echo "The only allowed parameter to this script is RANDOM or LIMITED or QUEUELIMITEDGENERAL."
    echo "Like $0 RANDOM"
    exit 1
fi


FAIL=0

# Start the service
if [[ $# -gt 0 && $1 == "RANDOM" ]]; then
    export VSOMEIP_CONFIGURATION=big_payload_test_local_random.json
elif [[ $# -gt 0 && $1 == "LIMITED" ]]; then
    export VSOMEIP_CONFIGURATION=big_payload_test_local_limited.json
elif [[ $# -gt 0 && $1 == "QUEUELIMITEDGENERAL" ]]; then
    export VSOMEIP_CONFIGURATION=big_payload_test_local_queue_limited.json
else
    export VSOMEIP_CONFIGURATION=big_payload_test_local.json
fi
./big_payload_test_service $1 &

# Start the client
./big_payload_test_client $1 &

# Wait until client and service are finished
for job in $(jobs -p)
do
    # Fail gets incremented if either client or service exit
    # with a non-zero exit code
    wait $job || ((FAIL+=1))
done

# Check if client and server both exited successfully 
if [ $FAIL -eq 0 ]
then
    exit 0
else
    exit 1
fi

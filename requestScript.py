import json
import sys
import re
from tradernet import NtApi

if len(sys.argv) == 3:
    pub_ = sys.argv[1]
    sec_ = sys.argv[2]

    cmd_ = 'getPositionJson'

    res = NtApi(pub_, sec_, NtApi.V2)
    json_response = res.sendRequest(cmd_)
    json_string = json.dumps(json_response)
    json_string = json_string.replace("'", '"')

    print(json_string)
else:
    print("No arguments provided")

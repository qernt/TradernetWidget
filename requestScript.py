import json
import sys
from tradernet import NtApi

if len(sys.argv) == 3:
    pub_ = sys.argv[1]
    sec_ = sys.argv[2]
else:
    print("No arguments provided")

cmd_ = 'getPositionJson'

res = NtApi(pub_, sec_, NtApi.V2)
print(res.sendRequest(cmd_))

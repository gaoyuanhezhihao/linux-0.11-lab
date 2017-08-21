#!/usr/bin/python3

import re

produced_set = set()
consumed_set = set()
cur_val_set = set()


with open("./usr_code/shm/print.txt") as f:
    lines = f.readlines()
    for i, l in enumerate(lines):
        name, pos, _, val, *_ = re.split(":|,| ", l)
        pos = int(pos)
        val = int(val)
        if name[0] == 'P':
            if val in produced_set:
                print("dupal '", val, "' founded")
                raise Exception("dupal produced val")
            produced_set.add(val)
            cur_val_set.add(val)
        else:
            if val not in cur_val_set:
                print("consume '", val, "' not produced yet")
                raise Exception("Non-exist consume")
            cur_val_set.remove(val)
            if val in consumed_set:
                print("dupal '", val, "' founded")
                raise Exception("dupal produced val")
            consumed_set.add(val)
print("Success!")

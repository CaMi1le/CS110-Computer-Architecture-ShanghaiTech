import sys

inp = sys.stdin.read()
inp_list = inp.split('\n')

for ln in inp_list:
    if ln == '.text' or len(ln) == 0:
        continue
    elif ln == '.symbol':
        break

    print('{:032b}'.format(int(ln, base=16)))
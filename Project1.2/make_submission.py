import sys, os, re

if __name__ == "__main__":
    modules = [ "string", "parsetools", "symbol_list" ]
    for module in modules:

        if not os.path.isdir(module):
            os.mkdir(module)

        filename = module + ".s"
        start_points = []
        funcnames = []
        with open(filename, 'r') as f:
            lines = f.readlines()
            for l in range(len(lines)):
                line = lines[l].strip()
                if line == "### YOUR CODE ###" or line == "### PASTE YOUR CODE IN string.s HERE ###":
                    start = l - 1
                    c = 0
                    for i in range(l, -1, -1):
                        if re.match(r'#-{20,}', lines[i].strip()):
                            c += 1
                            if c == 3:
                                start = i
                                break
                    funcname = lines[l - 1].strip().strip(':')
                    start_points.append(start)
                    funcnames.append(funcname)
                elif re.match(r'#{20,}', line):
                    start_points.append(l)
                    break

            for i in range(1, len(start_points)):
                start = start_points[i-1]
                end = start_points[i]
                funcname = funcnames[i-1]

                skip = False
                blank = 0
                out_lines = []
                for l in range(start, end):
                    line = lines[l]
                    if line.strip() == "":
                        blank += 1
                    elif line.strip() == "### PASTE YOUR CODE IN string.s HERE ###":
                        skip = True
                        break
                    else:
                        for j in range(blank): out_lines.append("\n")
                        out_lines.append(line)
                        blank = 0

                if not skip:
                    with open(module + '/' + funcname + ".s", 'w') as out:
                        if not out_lines[-1].endswith('\n'): out_lines[-1].append('\n')
                        out.writelines(out_lines)

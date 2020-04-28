# LinkerUtilities

A set of tools to help linker which combine code and relocate symbols, including implementation of string utilities, symbol list and parser tool, written in RISC-V.

## Specification

1. State the registers changed by function in the documentation comment area, with tag `CLOBBERS`. Refer to the pre-defined functions, like `print_newline`, for an instance.

2. Save return values in `a0` register.

3. Add prefix for labels. Due the uniqueness requirement of labels in RISC-V, the names of two labels should never be the same. To help manage these labels in separate files, add `str` prefix for labels defined in `string.s`, `sym` prefix for labels defined in `symbol_list.s` and `par` prefix for labels defined in `parsetools.s`. Prefixes should be concatenated with the label name by an underline(i.e `_`). For example, label named `loop_start` in `string.s` should finally be named as `str_loop_start`.

## External Links

1. [Project 1-2: RISC-V linker utilities](https://robotics.shanghaitech.edu.cn/courses/ca/19s/projects/1.2/)
2. [Course Page](https://robotics.shanghaitech.edu.cn/courses/ca/19s/)
3. [RISC-V Green Sheet](https://robotics.shanghaitech.edu.cn/courses/ca/19s/notes/riscvcard.pdf)
4. [Venus](https://venus.cs61c.org/)
5. [Environmental Calls](https://github.com/ThaumicMekanism/venus/wiki/Environmental-Calls)
6. [ASCII Table](https://www.cs.cmu.edu/~pattis/15-1XX/common/handouts/ascii.html)
<!--7. [Submission](http://autolab.shanghaitech.edu.cn/courses/Computer-Architecture-I-2019/assessments/project11riscvassembler)-->

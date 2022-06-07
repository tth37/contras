# CDL Standard 硬件描述语言标准

用一个 CDL 源文件描述一个元件，包括了它的内部逻辑、输入输出等信息。

CDL 源文件由四部分组成：导入部分、头部部分、逻辑主体部分以及结束部分。

```cdl
import AND from "AND.cdl"
import OR from "OR.cdl"
import NOT from "NOT.cdl"

@Head
NAME RSFlipFlop
INPUT S, R
OUTPUT Q, NQ
STATE q = 0, nq = 0
SYMBOL NAND NAND1, NAND2

@Body
NAND1(a = S, b = nq) => (out = res1)
NAND2(a = R, b = q) => (out = res2)

@End
STATE q = res1, nq = res2
OUTPUT Q = res1, NQ = res2
```

# Input File Standard 测试输入文件标准

一个输入文件由若干个 Case 组成，每一个 Case 将会由同一个实例依次运行并得到输出结果。

```in
@Case
S = 010100010011
R = 101111111101
Q = ?
NQ = ?

@Case
S[0] = 0
R = 1
Q = ?
```

经过仿真的输出文件样例如下：

```out
@Case
S[0]  = 010100010011
R[0]  = 101111111101
Q[0]  = 010101010101
NQ[0] = 101010101010

@Case
S[0] = 0
R[0] = 1
Q[0] = 0
```
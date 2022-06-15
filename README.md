# Contras HDL Simulator 硬件描述语言功能仿真工具

这是东南大学程序设计基础及语言 II 2021-2022 学年第二学期课程设计。

[开题报告](https://github.com/tth37/contras/blob/main/doc/%E3%80%8C%E7%AE%80%E6%98%93%E7%A1%AC%E4%BB%B6%E6%8F%8F%E8%BF%B0%E8%AF%AD%E8%A8%80%E5%8A%9F%E8%83%BD%E4%BB%BF%E7%9C%9F%E5%B7%A5%E5%85%B7%E3%80%8D%E5%BC%80%E9%A2%98%E6%8A%A5%E5%91%8A.pdf)

[答辩](https://github.com/tth37/contras/blob/main/doc/Contras%20HDL%20Simulator.pdf)

[开发者文档](https://github.com/tth37/contras/blob/main/doc/developerguide.md)

## 发布

empty now..

## 从源代码编译

无需安装依赖库。

```shell
$ cd contras
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

（可选）开启调试模式：

```shell
$ cmake .. -DCONTRAS_USE_LOGGER=1
$ cmake --build .
```

## 使用方法

参考 [CDL Language Standard](https://github.com/tth37/contras/blob/main/doc/cdlstandard.md)。

```shell
$ ./contras.exe <cdl_file> <input_file> <output_file>
```



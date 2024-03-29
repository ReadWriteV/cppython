# cppython

使用C++实现Python解释器。

根据《自己动手写Python虚拟机》一书，实现的Python解释器。相较于原书实现的版本，有以下特点：

+ 支持Python3。原书成书较早，实现的是Python2解释器，而如今Python3使用更为广泛，故改为实现Python3解释器。Python2和Python3的字节码部分有所区别，一些字节码是我推测实现的，可能有错误。

+ 使用现代C++实现。在实现过程中使用了现代C++风格，甚至是最新的C++标准，故对编译器要求较高，但是可以稍加修改以适配低版本的编译器。

## 构建

本项目使用 `cmake`管理，需要一个实现了部分C++23功能的C++编译器，不依赖任何第三方库。构建方法如下：

``` bash
cd cppython

cmake -S . -B build

cmake --build build
```

## 测试
在`test`文件夹下编写待测试的Python程序，然后保存`test\CMakeLists.txt`，触发cmake重新生成test目标，然后通过下面的命令运行所有测试用例：

``` bash
cd cppython

ctest --test-dir build
```

## 未来的工作

+ f-Strings。
+ 实现Python3前端。完成从.py源文件到.pyc的转换，对接到解释器，实现一个完整的流程。
+ 重构代码。这个项目也是我练习C++编码的途径，在不断学习C++的过程中，我也会考虑重构这个项目的代码，增加可读性、健壮性。
+ 目前在Windows上构建时，无法import DLL。因为拓展的 DLL 库会静态链接code、object、runtime等静态库，这些库中的静态变量（如integer_klass::instance）会在 DLL 和 主程序cppython.exe各有一份，导致无法通过判断klass来确定类型。


## 参考资料

+ [自己动手写Python虚拟机](https://book.douban.com/subject/34442805/)
+ [pythonvm](https://gitee.com/hinus/pythonvm)
+ [cpython](https://github.com/python/cpython)
+ [Python 3.9 字节码解释](https://docs.python.org/3.9/library/dis.html)
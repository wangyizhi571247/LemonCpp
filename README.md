# LemonCpp

LemonCpp 是一个简易的 C++ 代码编辑器，使用 C++ 编写，基于 ege 库实现。

可以在 CP（competitive programming）中使用。

运行时会有两个窗口，一个为代码编辑器，一个为控制台窗口（用于输入）。

不定期更新。

## 编译

将 `makefile/makefile.bat` 与 `src/` 中的所有文件放在同一目录下，运行 `makefile.bat`。

## 配置

运行时需要读取 `LemonCpp.config` 和 `LemonCppColor.config`（如果没有为默认值），可以从 `sample_config/` 中下载作者认为比较好看的配色与配置。

## 快捷键

选中使用 shift+方向键。

`Esc`：退出。

`Ctrl+N`：新建文档。

`Ctrl+O`：打开文档。

`Ctrl+S`：保存文档。

`Ctrl+D`：删除当前行。

`Ctrl+E`：将当前行复制插入到当前行后。

`Ctrl+C`：复制选中文本。

`Ctrl+V`：将剪切板中的内容复制进编辑器。

`Ctrl+Z`：撤销。

`Ctrl+R`：打开 `cmd`。

`F9`：编译。

`F11`：编译并运行。

`Ctrl+W`：强制终止编译。

`Ctrl+J`：跳转到其他行。

`Ctrl+/`：切换当前行或选中行的注释状态。

`Ctrl+up/down`：上/下滚动。

`Ctrl+Shift+up/down`：上/下移动当前行或选中行。

## 特殊功能

### Extester

用于测试样例的工具。

快捷键：`F12`。

使用方式：

在代码中加入 **多行注释**：

```cpp
/*
<ex>
{testcase}
{...testcases}
</ex>
*/
```

其中 `{testcase}` 格式为：

```
[tc] {name}
[in]
{input text}
[out]
{output text}
```

然后 Exteser 会自动测试这些样例并给出结果。

这个功能正在逐步完善中。
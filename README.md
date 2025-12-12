由@渡鸦2007开发的Amazon/亚马逊棋机器人。用于计概大作业。关注作者谢谢喵~ Star本项目谢谢喵~

主要使用的是：Minimax搜索，AlphaBeta优化，PVS零窗口优化，迭代加深，预排序并限制搜索分支数。

下一步的优化目标（很可能没有精力真的去写）：开局库，UI优化，常数优化。

Github的代码可能不会及时更新，但其应该不需要什么更新，已是较为完全的版本。

文件中：
- ~~amazon-super_ver.cpp 是可用于在 Botzone 上运行的 Bot。~~ 该文件已过时。

- amazon-PVS_ver.cpp 是最新的成果，引入了PVS优化且修复了一些bug。

- UI界面在文件夹 UIandBOT 中。Amazon-UI.cpp 是核心UI程序。其它存在的无关文件，如数据生成程序 Generate_Data 是为 Bot 准备的，可以忽略。Amazon-Bot.cpp应该由主文件夹的代码修改而得。

- 如果你有任何疑问都可以询问作者。

~~如果你是25届的北京大学学生且选修了计算概论A，请不要抄袭我的代码。~~

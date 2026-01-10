由@渡鸦2007开发的Amazon/亚马逊棋机器人。用于计概大作业。关注作者谢谢喵~ Star本项目谢谢喵~

主要使用的是：Minimax搜索，AlphaBeta优化，PVS零窗口优化，迭代加深，预排序并限制搜索分支数。

下一步的优化目标（很可能没有精力真的去写）：开局库，常数优化，美化图形化界面。

Github的代码可能不会及时更新，但其应该不需要什么更新，已是较为完全的版本。

文件中：
- ~~amazon-super_ver.cpp 是可用于在 Botzone 上运行的 Bot。~~ 该文件已过时。

- amazon-PVS_ver.cpp 是最新的成果，引入了PVS优化且修复了一些bug。这是 BotZone 上的版本119.截至目前(2026.01.10)，其天梯分数为1601.38.

- 实验报告.pptx 简要展示了本项目的亮点。

- UI界面在文件夹 UIandBOT 中。Amazon-UI.cpp 是控制台版本（字符实现）的核心UI程序。其它存在的无关文件，如数据生成程序 Generate_Data 是为 Bot 准备的，可以忽略。Amazon-Bot.cpp应该由主文件夹的代码修改而得。

- 现已更新图形化UI，使用 EasyX，具体请见 Amazon-UI_EasyX_ver.cpp。这是更好的程序，实现了点击落子、按钮控制。

- 如果你有任何疑问都可以询问作者。

~~如果你是25届的北京大学学生且选修了计算概论A，请不要抄袭我的代码。~~

下面是实际成果图片：

<img width="1201" height="1232" alt="屏幕截图 2025-12-25 172710" src="https://github.com/user-attachments/assets/e78fe8a7-6ad2-442c-829f-9e9d2eac8367" />

<img width="1235" height="673" alt="image" src="https://github.com/user-attachments/assets/ef9fa0fe-7dbe-4524-a582-9bac67013499" />

<img width="1239" height="672" alt="image" src="https://github.com/user-attachments/assets/a1068dff-2529-4e5d-94fa-a597884a45b0" />

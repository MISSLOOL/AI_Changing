# AI_Changing-UE5.5 开源插件:基于 Ollama 的 AI 聊天窗口
## 概述
AI_Changing 是专为 Unreal Engine 5.5 开发的开源插件，集成 Ollama 强大功能，为开发者打造便捷的 AI 聊天窗口解决方案。<br>
## 特性
Ollama 集成：与 Ollama 无缝衔接，借助其丰富 AI 模型开展对话交互。<br>
用户友好的聊天窗口：打造直观聊天界面，便于用户与 AI 交流。<br>
高度可定制：能依据项目需求，定制聊天窗口的外观与行为。<br>
易于集成：安装与配置流程简便，可快速集成到现有 UE5.5 项目中 。<br>
## 安装<br>
![image](public/action.png)
### 前提条件
Unreal Engine 5.5
### 步骤
1.从 GitHub 上克隆或下载插件代码至本地。<br>
2.将AI_Changing\Resources创建一个名为“AI_ZD”的文件夹<br>
3.然后从“https://github.com/ollama/ollama/releases/tag/v0.6.4” 下载win版本的ollama<br>
4.解压并发将名为“ollama”的文件夹放置在“AI_ZD”的文件夹下面<br>
  &ensp;&ensp;如果你不想：进行2，3，4步骤，可以直接下载整个插件：<br>
   &ensp;&ensp;链接: https://pan.baidu.com/s/1-mMF_3GmJ4Vn8_rHBRmfdw 提取码: 9420 <br>
5.将整个插件文件夹复制到 UE5.5 项目的Plugins目录下。<br>
6.打开 UE5.5 编辑器，在Edit -> Plugins中找到 AI_Changing 插件并启用。<br>
7.重启 UE5.5 编辑器。
## 配置
1.打开 UE5.5 编辑器，在 “编辑器偏好” 的 “插件” 的 “AI chat” 开源设置中，找到 AI_Changing 插件的配置选项。<br>
2.默认已配置 Ollama 的 “API” 地址和 “11434” 端口。<br>
## 使用方法
1.点击编辑器工具栏上方的图标按钮，或在窗口下拉菜单最下方的图标按钮，即可调出聊天窗口。<br>
2.按需选择合适 AI 模型，若没有模型，点击启动服务后会自动下载相应模型。<br>
3.用户可在输入框输入消息，与 AI 对话。<br>
## 贡献
欢迎社区贡献！若有改进建议、bug 修复或新功能设想，可通过以下方式参与：<br>
提交 issue：在 GitHub 的 issue 页面提出问题或建议。<br>
提交 pull request：fork 本项目，修改后提交 pull request。<br>
## 许可证
本插件采用 [开源许可证名称] 许可证，详情参阅LICENSE文件。<br>

# AI_Changing 插件图标

此目录包含AI_Changing插件使用的图标资源。

## SVG图标文件
- `icon_AI_40x.svg` - 40x40分辨率的AI图标，用于工具栏和菜单
- `icon_AI_16x.svg` - 16x16分辨率的AI图标，用于小尺寸显示

## PNG图标文件(备用)
如果SVG格式在某些UE版本中不受支持，请使用以下PNG格式图标：
- `icon_AI_40x.png` - 40x40分辨率的AI图标
- `icon_AI_16x.png` - 16x16分辨率的AI图标

## 使用方法
这些图标已通过FAI_ChangingStyle类中的以下设置进行引用：
```cpp
Style->Set("AI_Changing.OpenAIDialog", new IMAGE_BRUSH("Icons/icon_AI_40x", Icon40x40));
Style->Set("AI_Changing.OpenAIDialog.Small", new IMAGE_BRUSH("Icons/icon_AI_16x", Icon16x16));
```

## 自定义图标
如需替换图标，只需保持文件名不变，替换相应的SVG或PNG文件即可。
SVG格式的优点是可缩放且文件较小，但如果遇到兼容性问题，可使用PNG格式。 
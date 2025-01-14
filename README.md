<h1 align="center">sWelcomer</h1>
<p align="center">A simple, lightweight quick-access welcomer</p>

sWelcomer is a simple welcomer window with quick-access to several tools written in C with SDL2\
It supports configurable entries, random greet text and footer

![screenshot_0](./img/screenshot_0.png)\
(Default unconfigured state)

# Installing

```bash
git clone https://github.com/danilaxila3/sWelcomer.git
cd sWelcomer
make
```

After that you can add the `bin/swelcomer` file to your programs on startup (`exec-once` or `shell:startup` on Windows)

# Config

The config is found in these dirs:\
If `$XDG_CONFIG_HOME` is set, the dir is `$XDG_CONFIG_HOME/sWelcomer/swelcomer.scf`\
If the variable is not set:
- Windows: `%LOCALAPPDATA%\sWelcomer\swelcomer.scf`
- Linux: `$HOME/.config/sWelcomer/swelcomer.scf`

Config structure:

```
[general]
window_factor = 3.0
fade_window = true
color_active = #e6e6e6
color_inactive = #b3b3b3
color_bg = #18181b

[options]
󰈹 Firefox; firefox
󰗃 YouTube; firefox https://youtube.com
 Spotify; firefox https://open.spotify.com
󰨞 VS Code; code

[greets]
greet 1
greet 2
greet 3

[footers]
footer 1
footer 2
footer 3
```

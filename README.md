Fork of [KDE/rsibreak](https://github.com/KDE/rsibreak) with some changes so it works better for me.

This only works on KDE Plasma on Wayland.
Set the environment variable `QT_QPA_PLATFORM=xcb` prior to running.
Config file is at `$XDG_CONFIG_HOME/rsibreakrc` (by default `~/.config/rsibreakrc`).

Working:
- Big breaks are optional.
- Idle detection is working on Wayland.
- Break countdown is displayed in center of screen and with better formatting.

Bugs:
- Keyboard input to other apps is not blocked during a break. Fixing this will probably require a re-write as a native Wayland application, which I don't plan to do for now.
- Configuration > During Breaks > Popup section can become irreversably blocked out. To get it back, edit the config file.

To use as a Nix overlay:
```nix
(final: prev: {
    rsibreak = prev.rsibreak.overrideAttrs (old: {
        version = "xwayland-0.13.0";

        src = final.fetchFromGitHub {
            owner = "alorans";
            repo = "rsibreak_xwayland";
            rev = ""; # copy the most recent commit hash
            hash = ""; # nix will tell you what to put if you leave it blank
        }

        nativeBuildInputs = (old.nativeBuildInputs or []) ++ [
            final.pkg-config
            final.wayland-scanner
        ];
        buildInputs = (old.buildInputs or []) ++ [
            final.wayland
            final.wayland-protocols
        ];

        postFixup = (old.postFixup or "") + ''
            wrapProgram $out/bin/rsibreak --set QT_QPA_PLATFORM xcb
        '';
    });
})
```

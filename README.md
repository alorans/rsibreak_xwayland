Fork of [KDE/rsibreak](https://github.com/KDE/rsibreak) that works  on Wayland (via Xwayland).

This is only meant for KDE Plasma Wayland.
Set the environment variable `QT_QPA_PLATFORM=xcb` prior to running.
Config file is at `$XDG_CONFIG_HOME/rsibreakrc` (by default `~/.config/rsibreakrc`).

Changes:
- Big breaks are optional.
- Idle detection works on Wayland.
- Better formatting for the break countdown widget.

Bugs:
- Keyboard input to other apps is not blocked during a break. Fixing this properly would require a re-write as a native Wayland screenlocker application, which I don't plan to do for now.

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

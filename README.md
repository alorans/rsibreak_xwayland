Fork of [KDE/rsibreak](https://github.com/KDE/rsibreak) with some changes so it works better for me.

Working:
- Big breaks are optional.
- Idle detection is working on Wayland.
- Break countdown is displayed in center of screen and with better formatting.

Bugs:
- Keypresses are not blocked during a break on Wayland.
- Configuration > During Breaks > Popup section becomes irreversably blocked out (you must get it back by editing the config file).

This only works on KDE Plasma using Wayland.
You must set the environment variable `QT_QPA_PLATFORM=xcb` prior to running.

This primarily exists so I can use it in an overlay in my Nix config:
```nix
(final: prev: {
    rsibreak = prev.rsibreak.overrideAttrs (old: {
    version = "unstable-0.13.0";

    src = final.fetchFromGitHub {
        owner = "alorans";
        repo = "rsibreak";
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
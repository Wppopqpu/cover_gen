# **cover_gen**

## a tool for generating `alpha-nvim` images.

---

### Screenshot

![screenshot](https://raw.github.com/wppopqpu/cover_gen/main/assets/screenshot.png)

### Installation

```sh
xmake -v && xmake install -o ~/.local/stow/cover_gen
stow -d.. -v2 -S cover_gen
```
(make sure your compiler supports **modules**!)

### Usage

```sh
cover_gen IMAGE OUTPUT_WIDTH OUTPUT_HEIGHT | TARGET_FILE
```

it will generate a lua module contains the following items:

- `text`
- `hl`

pass them to your `alpha`.

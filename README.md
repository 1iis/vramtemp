# `vramtemp`
> VRAM Temperature Reader

Ampere and Ada don't report VRAM temperatures in official NVIDIA Linux tools such as `nvidia-smi` / `nvidia-settings`. So we work around it by reading directly from the driver's outputs, with a dead simple C program, easy to consume by other scripts and applications. Tested on RTX 3090.

> [!Note]  
> **This is a barebone rewrite of [`gddr6`](https://github.com/olealgoritme/gddr6) by Ole Algoritme** ([@olealgoritme](https://github.com/olealgoritme)).  
> The purpose of `vramtemp` is compliance with stdio: stdout (`>1`) is a single reading then exits.
> - Works with pipes `|` `>` `&&` etc.
> - Consumable by scripts.
> - Non-blocking for the terminal session. Pipe with `watch` or ` log ` to monitor continuously.
> 
> Core logic stays the same.

## Build
```bash
gcc -o vramtemp vramtemp.c -lpci
```

## Usage

By default, outputs values in one line, space-separated.
```bash
sudo ./vramtemp
```
↓
```
76 42
```

Flag `-l` to get one GPU per line.
```
sudo ./vramtemp -l
```
↓
```
0 76
1 42
```

Use `awk` or `cut` to get your desired value.

```bash
sudo vramtemp | awk '{print $1}'   #→ 76
sudo vramtemp | awk '{print $2}'   #→ 42
sudo vramtemp | cut -d' ' -f1      #→ 76
sudo vramtemp | cut -d' ' -f2      #→ 42
```

## Why `vramtemp`?

- Fast (written in C).
- Minimal dependencies: NVML is not required (no nvml.h, no CUDA).
- Clean output then exit. Use e.g. `watch -n 0.5` or similar tool to get a continuous read.
- Tool-friendly: works with scripts, pipes, etc.
- Extremely small: less than 2,000 chars (1.8KB).

## Thanks
**The heavy lifting was entirely done by Ole Algoritme** ([@olealgoritme](https://github.com/olealgoritme)) in [`gddr6`](https://github.com/olealgoritme/gddr6), reverse-engineering the NVIDIA driver!  
I only trimmed it for my use case (with [Grok 4.3 (beta)](https://grok.com/)).

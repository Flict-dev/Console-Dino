import shutil

for i in range(68, 91):
    with open(f"{i}.bmp", "x") as f:
        shutil.copy2("67.bmp", f"{i}.bmp")

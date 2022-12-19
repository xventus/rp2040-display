# Font Generator utility

It generates characters from a TTF font file and creates a header for the C language, usable for displays.


# How to install 
```
python3 -m pip install Pillow
python3 -m pip install bitstring
python3 ./font_generator.py 
```
# Free font source

example:
```
fnt = FontGenerator("./source/CodeSquaredRegular-AYRg.ttf", 10, 7, 10, -2, 100)
fnt.withRangeChars('0', '9')
fnt.generate("./generated/codeSquaredRegular")
```


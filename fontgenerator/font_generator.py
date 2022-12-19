# Copyright (c) 2022 Petr Vanek, petr@fotoventus.cz
# as source for conversion is TTF, OTF  file with font
#


from PIL import ImageDraw, Image, ImageFont
import math
import os.path


class FontGenerator:

    def __init__(self, fontFile, fontSize, width=8, height=8, ycorrection=0, threshold=255):
        self.charsToConvert = "A"
        self.filename = fontFile
        self.width = width
        self.height = height
        self.fontSize = fontSize
        self.ycorrection = ycorrection
        self.threshold = threshold

    def withFontSize(self, fontSize):
        self.fontSize = fontSize

    def withYCorrection(self, correction):
        self.ycorrection = correction

    def withConvertChars(self, charString):
        self.charsToConvert = charString

    def withRangeChars(self, fromChar, toChar):
        self.charsToConvert = ''.join(chr(chrs) for chrs in range(ord(fromChar), ord(toChar) + 1))

    def withCharSize(self, width, height):
        self.width = width
        self.height = height

    def withFontFile(self, filename):
        self.filename = filename

    def generate(self, filePath):
        outputBaseName = os.path.splitext(filePath)[0]
        image = Image.new("RGBA", (len(self.charsToConvert) * (self.width + 1), self.height), (255, 255, 255))
        loadedFont = ImageFont.truetype(self.filename, self.fontSize)
        draw = ImageDraw.Draw(image)

        for pos in range(len(self.charsToConvert)):
            draw.text((pos * (self.width + 1), self.ycorrection), self.charsToConvert[pos], font=loadedFont,
                      fill=(0, 0, 0, 255))

        image.save(outputBaseName + '.png')

        f = open(outputBaseName + '.h', 'w')
        numOfChars = len(self.charsToConvert)
        f.write('/*Generated font data*/\n\n')
        f.write('#pragma once\n\n')
        f.write('namespace  %s {\n\n' % os.path.split(filePath)[1])
        f.write('\tstatic const uint8_t glypWidth = %d;\n' % self.width)
        f.write('\tstatic const uint8_t glypHeight = %d;\n' % self.height)
        f.write('\tstatic const uint8_t glyps = %d;\n' % (numOfChars))
        f.write('\tstatic const unsigned char firstChar = \'%c\';\n' % self.charsToConvert[0])
        f.write('\n')

        f.write('\tstatic const unsigned char fnt[%d][%d] = {\n' % (numOfChars + 1, self.width * math.ceil(self.height / 8)))

        chars = []
        for pos in range(numOfChars):
            dataArray = []

            for xIter in range(self.width):
                x = pos * (self.width + 1) + xIter
                acumulator = 0
                writeCounter = 0
                for y in range(self.height):
                    pixelColor = image.getpixel((x, y))
                    if pixelColor[0] <= self.threshold:
                        acumulator = (acumulator >> 1) | 0x80
                    else:
                        acumulator = (acumulator >> 1)

                    writeCounter += 1

                    if writeCounter == 8:
                        writeCounter = 0
                        dataArray.append('0x%.2x' % acumulator)
                        acumulator = 0

                if writeCounter != 0 & writeCounter !=8:
                    rotate = 8 - writeCounter
                    acumulator = acumulator >> rotate
                    dataArray.append('0x%.2x' % acumulator)

            c = self.charsToConvert[pos]
            if c == '\\':
                c = '"\\"'
            f.write('\t\t{%s}, //  >%s<\n' % (','.join(dataArray), c))

        f.write('};\n\n')
        f.write('} // namespace\n')
        f.close()


if __name__ == "__main__":

    fnt = FontGenerator("./source/CodeSquaredRegular-AYRg.ttf", 10, 7, 10, -2, 100)
    fnt.withRangeChars('0', '9')
    fnt.generate("./../src/generated/codeSquaredRegular")

    fnt = FontGenerator("./source/Modeseven-L3n5.ttf", 10, 6, 9, -1, 120)
    fnt.withRangeChars(' ', '~')
    fnt.generate("./../src/generated/modeseven")

    fnt = FontGenerator("./source/Topaz8-xxO8.ttf", 9, 7, 8, 0, 100)
    fnt.withRangeChars(' ', '~')
    fnt.generate("./../src/generated/topaz")





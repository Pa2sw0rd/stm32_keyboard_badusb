value=''''
win'+r
cmd
'enter'
echo a
'''
finalValue=None
def checkControl(value):
    if "'" in value:
        return value[1:(len(value)-1)]
    else:
        return False
def main():
    countStep=0
    stepList=value.split("\n")
    for step in stepList:
        if '+' in step:
            a=step.split("+")
            for b in a:
                if checkControl(b)!=False:
                    finalValue[countStep][keyCode[b],0x00,0x00,0x00,0x00,0x00,0x00,0x00]
            
            


if __name__=="__main__":
    main()

keyCode={
#NULL 0x00          # NULL
"A":"0x04",             # A
"B":"0x05",             # B
"C":"0x06",             # C
"D":"0x07",             # D
"E":"0x08",             # E
"F":"0x09",             # F
"G":"0x0A",             # G
"H":"0x0B",             # H
"I":"0x0C",             # I
"J":"0x0D",             # J
"K":"0x0E",             # K
"L":"0x0F",             # L
"M":"0x10",             # M
"N":"0x11",             # N
"O":"0x12",             # O
"P":"0x13",             # P
"Q":"0x14",             # Q
"R":"0x15",             # R
"S":"0x16",             # S
"T":"0x17",             # T
"U":"0x18",             # U
"V":"0x19",             # V
"W":"0x1A",             # W
"X":"0x1B",             # X
"Y":"0x1C",             # Y
"Z":"0x1D",             # Z
"1":"0x1E",             # 1 and !
"2":"0x1F",             # 2 and @
"3":"0x20",             # 3 and #
"4":"0x21",             # 4 and $
"5":"0x22",             # 5 and %
"6":"0x23",             # 6 and ^
"7":"0x24",             # 7 and &
"8":"0x25",             # 8 and *
"9":"0x26",             # 9 and (
"0":"0x27",             # 10 and )
"ENTER":"0x28",         # ENTER
"ESC":"0x29",           # ESC
"BACKSPACE":"0x2A",     # BACKSPACE
"TAB":"0x2B",           # TAB
"SPACE":"0x2C",         # SPACE
"SUB":"0x2D",           # - and _
"EQUAL":"0x2E",         # = and +
"LEFT_BRACKET":"0x2F",  # [ and {
"RIGHT_BRACKET":"0x30", # ] and }
"VERTICAL_LINE":"0x31", # "\" and |
"WAVE":"0x32",          # ` and ~
"SEMICOLON":"0x33",     # ; and :
"QUOTE":"0x34",         # ' and "
"THROW":"0x35",         # ~ and `
"COMMA":"0x36",         # , and <
"DOT":"0x37",           # . and >
"QUESTION":"0x38",      # / and ?
"CAPS_LOCK":"0x39",     # CAPS
"F1":"0x3A",
"F2":"0x3B",
"F3":"0x3C",
"F4":"0x3D",
"F5":"0x3E",
"F6":"0x3F",
"F7":"0x40",
"F8":"0x41",
"F9":"0x42",
"F10":"0x43",
"F11":"0x44",
"F12":"0x45",
"PRT_SCR":"0x46",
"SCOLL_LOCK":"0x47",
"PAUSE":"0x48",
"INS":"0x49",
"HOME":"0x4A",
"PAGEUP":"0x4B",
"DEL":"0x4C",
"END":"0x4D",
"PAGEDOWN":"0x4E",
"RIGHT_ARROW":"0x4F",
"LEFT_ARROW":"0x50",
"DOWN_ARROW":"0x51",
"UP_ARROW":"0x52",
#Num Pad":"",
"PAD_NUMLOCK":"0x53",
"PAD_DIV":"0x54",
"PAD_MUL":"0x55",
"PAD_SUB":"0x56",
"PAD_ADD":"0x57",
"PAD_ENTER":"0x58",
"PAD_1":"0x59",
"PAD_2":"0x5A",
"PAD_3":"0x5B",
"PAD_4":"0x5C",
"PAD_5":"0x5D",
"PAD_6":"0x5E",
"PAD_7":"0x5F",
"PAD_8":"0x60",
"PAD_9":"0x61",
"PAD_0":"0x62",
"PAD_DOT":"0x63",
"PRESSED":"0x00",
"RELEASED":"0x01",
# Control":"",
"LCTRL":"0xE0",  # left ctrl # #define LCTRL 0x01
"LALT":"0xE2",   # left Alt # #define LALT 0x04
"LSHFIT":"0xE1", # left Shift # #define LSHFIT 0x02
"LWIN":"0xE3",   # left windows # #define LWIN 0x08
"RWIN":"0xE7",   # right windows # #define RWIN 0x80
"RSHIFT":"0xE5", # right Shift # #define RSHIFT 0x20
"RALT":"0xE6",   # right Alt # #define RALT 0x40
"RCTRL":"0xE4",  # right Ctrl # #define RCTRL 0x10
"APP":"0x65",    # Application # #define APP 0x65
"K14":"0x89",    # international key
"KR_L":"0x91",
"K107":"0x85",
"K45":"0x64",
"K42":"0x32",
"K131":"0x8b",
"K132":"0x8a",
"K133":"0x88",
"K56":"0x87",
"KR_R":"0x90"
}
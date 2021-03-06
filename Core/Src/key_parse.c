#include "usbd_hid.h"
#include "usb_device.h"
#include "key_parse.h"
//连续三个相同有问题，需要先弹起
extern USBD_HandleTypeDef hUsbDeviceFS;
unsigned char ascii_to_key(unsigned char ascii){
    if(32<=ascii&&ascii<=126){
        if(0x41<=ascii&&ascii<=0x5a){//大写字母
            return ascii-0x3d;
        }
        if(0x61<=ascii&&ascii<=0x7a){//小写字母
            return ascii-0x5d;
        }
        if(0x30<=ascii&&ascii<=0x39){//数字
            if(ascii==0x30) return 0x27;
            return ascii-0x13;
        }
        switch (ascii)
        {
            case '-':return 0x2d;
            case '=':return 0x2e;
            case '[':return 0x2f;
            case ']':return 0x30;
            case ';':return 0x33;
            case 0x27:return 0x34;
            case 0x5c:return 0x31;
            case ',':return 0x36;
            case '.':return 0x37;
            case '/':return 0x38;
            case ' ':return KEY_SPACE;
            default:return 0;
        }
    }else{
        return 0;
    }
    return 0;
}
unsigned char parse_key(unsigned char key){
    //unsigned char temp=ascii_to_key(key);

    switch (key)
    {
        case '!':return ascii_to_key('1');break;
        case '@':return ascii_to_key('2');break;
        case '#':return ascii_to_key('3');break;
        case '$':return ascii_to_key('4');break;
        case '%':return ascii_to_key('5');break;
        case '^':return ascii_to_key('6');break;
        case '&':return ascii_to_key('7');break;
        case '*':return ascii_to_key('8');break;
        case '(':return ascii_to_key('9');break;
        case ')':return ascii_to_key('0');break;
        case '_':return ascii_to_key('-');break;
        case '+':return ascii_to_key('=');break;
        case '{':return ascii_to_key('[');break;
        case '}':return ascii_to_key(']');break;
        case ':':return ascii_to_key(';');break;
        case '"':return ascii_to_key(0x27);break;
        case '|':return ascii_to_key(0x5c);break;
        case '<':return ascii_to_key(',');break;
        case '>':return ascii_to_key('.');break;
        case '?':return ascii_to_key('/');break;
        default:return key;
    }

    return key;

    

}
void key_print(unsigned char *string){
/*
输出字符    abcd@@abd@a
*/
    unsigned int i,j,nextKey,temp;
    i=0;
    j=0;
    while(string[i]!='\0'){ 
        temp=ascii_to_key(string[i]);
        nextKey=ascii_to_key(string[i+1]);
        if(temp){
            key_data[2+j]=temp;
            j++;
            if(j==6){
                j=0;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }else if(!nextKey||parse_key(nextKey)==temp){
                j=0;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }
        }else{
            temp=parse_key(string[i]);
            key_data[2+0]=temp;
            j++;
            if(j==6){
                j=0;
                key_data[0]=KEY_SHIFT;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }else if(nextKey||nextKey==temp){
                j=0;
                key_data[0]=KEY_SHIFT;
                USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
                HAL_Delay(KEY_DELAY);
                key_unpress_all();
                HAL_Delay(KEY_DELAY);
            }
        }
        i++;
        //if(string[i]=='\0') return;
    }
}

void key_press(unsigned char key){
/*
按下功能键盘
*/
    key_data[0]=key;
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
    HAL_Delay(KEY_DELAY);
}
void key_unpress(void){
/*
弹起功能键盘
*/
    key_data[0]=0x00;
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
}
void key_unpress_all(void){
/*
弹起所有键
*/
    for(unsigned char i=0;i<8;i++){
        key_data[i]=0x00;
    }
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
}

void pressFunction(unsigned char key){
    key_data[2]=key;
    USBD_HID_SendReport(&hUsbDeviceFS,key_data,8);
    HAL_Delay(KEY_DELAY);
    key_unpress_all();
    HAL_Delay(KEY_DELAY);
}













/*
BYTE1 -- 特殊按键
        |--bit0:   Left Control是否按下，按下为1  
        |--bit1:   Left Shift  是否按下，按下为1  
        |--bit2:   Left Alt    是否按下，按下为1  
        |--bit3:   Left GUI（Windows键） 是否按下，按下为1  
        |--bit4:   Right Control是否按下，按下为1   
        |--bit5:   Right Shift 是否按下，按下为1  
        |--bit6:   Right Alt   是否按下，按下为1  
        |--bit7:   Right GUI   是否按下，按下为1 


BYTE2 -- 暂不清楚，有的地方说是保留位
BYTE3--BYTE8 -- 这六个为普通按键

第一列10进制键值，第二列16进制键值，第四列是按键  
   
0 00 Reserved (no event indicated)9 N/A √ √ √ 4/101/104  
1 01 Keyboard ErrorRollOver9 N/A √ √ √ 4/101/104  
2 02 Keyboard POSTFail9 N/A √ √ √ 4/101/104  
3 03 Keyboard ErrorUndefined9 N/A √ √ √ 4/101/104  
4 04 Keyboard a and A4 31 √ √ √ 4/101/104  
5 05 Keyboard b and B 50 √ √ √ 4/101/104  
6 06 Keyboard c and C4 48 √ √ √ 4/101/104  
7 07 Keyboard d and D 33 √ √ √ 4/101/104  
8 08 Keyboard e and E 19 √ √ √ 4/101/104  
9 09 Keyboard f and F 34 √ √ √ 4/101/104  
10 0A Keyboard g and G 35 √ √ √ 4/101/104  
11 0B Keyboard h and H 36 √ √ √ 4/101/104  
12 0C Keyboard i and I 24 √ √ √ 4/101/104  
13 0D Keyboard j and J 37 √ √ √ 4/101/104  
14 0E Keyboard k and K 38 √ √ √ 4/101/104  
15 0F Keyboard l and L 39 √ √ √ 4/101/104  
16 10 Keyboard m and M4 52 √ √ √ 4/101/104  
17 11 Keyboard n and N 51 √ √ √ 4/101/104  
18 12 Keyboard o and O4 25 √ √ √ 4/101/104  
19 13 Keyboard p and P4 26 √ √ √ 4/101/104  
20 14 Keyboard q and Q4 17 √ √ √ 4/101/104  
21 15 Keyboard r and R 20 √ √ √ 4/101/104  
22 16 Keyboard s and S4 32 √ √ √ 4/101/104  
23 17 Keyboard t and T 21 √ √ √ 4/101/104  
24 18 Keyboard u and U 23 √ √ √ 4/101/104  
25 19 Keyboard v and V 49 √ √ √ 4/101/104  
26 1A Keyboard w and W4 18 √ √ √ 4/101/104  
27 1B Keyboard x and X4 47 √ √ √ 4/101/104  
28 1C Keyboard y and Y4 22 √ √ √ 4/101/104  
29 1D Keyboard z and Z4 46 √ √ √ 4/101/104  
30 1E Keyboard 1 and !4 2 √ √ √ 4/101/104  
31 1F Keyboard 2 and @4 3 √ √ √ 4/101/104  
32 20 Keyboard 3 and #4 4 √ √ √ 4/101/104  
33 21 Keyboard 4 and $4 5 √ √ √ 4/101/104  
34 22 Keyboard 5 and %4 6 √ √ √ 4/101/104  
35 23 Keyboard 6 and ^4 7 √ √ √ 4/101/104  
36 24 Keyboard 7 and &4 8 √ √ √ 4/101/104  
37 25 Keyboard 8 and *4 9 √ √ √ 4/101/104  
38 26 Keyboard 9 and (4 10 √ √ √ 4/101/104  
39 27 Keyboard 0 and )4 11 √ √ √ 4/101/104  
40 28 Keyboard Return (ENTER)5 43 √ √ √ 4/101/104  
41 29 Keyboard ESCAPE 110 √ √ √ 4/101/104  
42 2A Keyboard DELETE (Backspace)13 15 √ √ √ 4/101/104  
43 2B Keyboard Tab 16 √ √ √ 4/101/104  
44 2C Keyboard Spacebar 61 √ √ √ 4/101/104  
45 2D Keyboard - and (underscore)4 12 √ √ √ 4/101/104  
46 2E Keyboard = and +4 13 √ √ √ 4/101/104  
47 2F Keyboard [ and {4 27 √ √ √ 4/101/104  
48 30 Keyboard ] and }4 28 √ √ √ 4/101/104  
49 31 Keyboard \ and | 29 √ √ √ 4/101/104  
50 32 Keyboard Non-US # and ~2 42 √ √ √ 4/101/104  
51 33 Keyboard ; and :4 40 √ √ √ 4/101/104  
52 34 Keyboard ‘ and “4 41 √ √ √ 4/101/104  
53 35 Keyboard Grave Accent and Tilde4 1 √ √ √ 4/101/104  
54 36 Keyboard, and <4 53 √ √ √ 4/101/104  
55 37 Keyboard . and >4 54 √ √ √ 4/101/104  
56 38 Keyboard / and ?4 55 √ √ √ 4/101/104  
57 39 Keyboard Caps Lock11 30 √ √ √ 4/101/104  
58 3A Keyboard F1 112 √ √ √ 4/101/104  
59 3B Keyboard F2 113 √ √ √ 4/101/104  
60 3C Keyboard F3 114 √ √ √ 4/101/104  
61 3D Keyboard F4 115 √ √ √ 4/101/104  
62 3E Keyboard F5 116 √ √ √ 4/101/104  
63 3F Keyboard F6 117 √ √ √ 4/101/104  
64 40 Keyboard F7 118 √ √ √ 4/101/104  
65 41 Keyboard F8 119 √ √ √ 4/101/104  
66 42 Keyboard F9 120 √ √ √ 4/101/104  
67 43 Keyboard F10 121 √ √ √ 4/101/104  
68 44 Keyboard F11 122 √ √ √ 101/104  
69 45 Keyboard F12 123 √ √ √ 101/104  
70 46 Keyboard PrintScreen1 124 √ √ √ 101/104  
71 47 Keyboard Scroll Lock11 125 √ √ √ 4/101/104  
72 48 Keyboard Pause1 126 √ √ √ 101/104  
73 49 Keyboard Insert1 75 √ √ √ 101/104  
74 4A Keyboard Home1 80 √ √ √ 101/104  
75 4B Keyboard PageUp1 85 √ √ √ 101/104  
76 4C Keyboard Delete Forward1;14 76 √ √ √ 101/104  
77 4D Keyboard End1 81 √ √ √ 101/104  
78 4E Keyboard PageDown1 86 √ √ √ 101/104  
79 4F Keyboard RightArrow1 89 √ √ √ 101/104  
80 50 Keyboard LeftArrow1 79 √ √ √ 101/104  
81 51 Keyboard DownArrow1 84 √ √ √ 101/104  
82 52 Keyboard UpArrow1 83 √ √ √ 101/104  
83 53 Keypad Num Lock and Clear11 90 √ √ √ 101/104  
84 54 Keypad /1 95 √ √ √ 101/104  
85 55 Keypad * 100 √ √ √ 4/101/104  
86 56 Keypad - 105 √ √ √ 4/101/104  
87 57 Keypad + 106 √ √ √ 4/101/104  
88 58 Keypad ENTER5 108 √ √ √ 101/104  
89 59 Keypad 1 and End 93 √ √ √ 4/101/104  
90 5A Keypad 2 and Down Arrow 98 √ √ √ 4/101/104  
91 5B Keypad 3 and PageDn 103 √ √ √ 4/101/104  
92 5C Keypad 4 and Left Arrow 92 √ √ √ 4/101/104  
93 5D Keypad 5 97 √ √ √ 4/101/104  
94 5E Keypad 6 and Right Arrow 102 √ √ √ 4/101/104  
95 5F Keypad 7 and Home 91 √ √ √ 4/101/104  
96 60 Keypad 8 and Up Arrow 96 √ √ √ 4/101/104  
97 61 Keypad 9 and PageUp 101 √ √ √ 4/101/104  
98 62 Keypad 0 and Insert 99 √ √ √ 4/101/104  
99 63 Keypad . and Delete 104 √ √ √ 4/101/104  
100 64 Keyboard Non-US \ and |3;6 45 √ √ √ 4/101/104  
101 65 Keyboard Application10 129 √ √ 104  
102 66 Keyboard Power9 √ √  
103 67 Keypad = √  
104 68 Keyboard F13 √  
105 69 Keyboard F14 √  
106 6A Keyboard F15 √  
107 6B Keyboard F16  
108 6C Keyboard F17  
109 6D Keyboard F18  
110 6E Keyboard F19  
111 6F Keyboard F20  
112 70 Keyboard F21  
113 71 Keyboard F22  
114 72 Keyboard F23  
115 73 Keyboard F24  
116 74 Keyboard Execute √  
117 75 Keyboard Help √  
118 76 Keyboard Menu √  
119 77 Keyboard Select √  
120 78 Keyboard Stop √  
121 79 Keyboard Again √  
122 7A Keyboard Undo √  
123 7B Keyboard Cut √  
124 7C Keyboard Copy √  
125 7D Keyboard Paste √  
126 7E Keyboard Find √  
127 7F Keyboard Mute √  
128 80 Keyboard Volume Up √  
129 81 Keyboard Volume Down √  
130 82 Keyboard Locking Caps Lock12 √  
131 83 Keyboard Locking Num Lock12 √  
132 84 Keyboard Locking Scroll Lock12 √  
133 85 Keypad Comma27 107  
134 86 Keypad Equal Sign29  
135 87 Keyboard International115,28 56  
136 88 Keyboard International216  
137 89 Keyboard International317  
138 8A Keyboard International418  
139 8B Keyboard International519  
140 8C Keyboard International620  
141 8D Keyboard International721  
142 8E Keyboard International822  
143 8F Keyboard International922  
144 90 Keyboard LANG125  
145 91 Keyboard LANG226  
146 92 Keyboard LANG330  
147 93 Keyboard LANG431  
148 94 Keyboard LANG532  
149 95 Keyboard LANG68  
150 96 Keyboard LANG78  
151 97 Keyboard LANG88  
152 98 Keyboard LANG98  
153 99 Keyboard Alternate Erase7  
154 9A Keyboard SysReq/Attention1  
155 9B Keyboard Cancel  
156 9C Keyboard Clear  
157 9D Keyboard Prior  
158 9E Keyboard Return  
159 9F Keyboard Separator  
160 A0 Keyboard Out  
161 A1 Keyboard Oper  
162 A2 Keyboard Clear/Again  
163 A3 Keyboard CrSel/Props  
164 A4 Keyboard ExSel  
165-175 A5-CF Reserved  
176 B0 Keypad 00  
177 B1 Keypad 000  
178 B2 Thousands Separator 33  
179 B3 Decimal Separator 33  
180 B4 Currency Unit 34  
181 B5 Currency Sub-unit 34  
182 B6 Keypad (  
183 B7 Keypad )  
184 B8 Keypad {  
185 B9 Keypad }  
186 BA Keypad Tab  
187 BB Keypad Backspace  
188 BC Keypad A  
189 BD Keypad B  
190 BE Keypad C  
191 BF Keypad D  
192 C0 Keypad E  
193 C1 Keypad F  
194 C2 Keypad XOR  
195 C3 Keypad ^  
196 C4 Keypad %  
197 C5 Keypad <  
198 C6 Keypad >  
199 C7 Keypad &  
200 C8 Keypad &&  
201 C9 Keypad |  
202 CA Keypad ||  
203 CB Keypad :  
204 CC Keypad #  
205 CD Keypad Space  
206 CE Keypad @  
207 CF Keypad !  
208 D0 Keypad Memory Store  
209 D1 Keypad Memory Recall  
210 D2 Keypad Memory Clear  
211 D3 Keypad Memory Add  
212 D4 Keypad Memory Subtract  
213 D5 Keypad Memory Multiply  
214 D6 Keypad Memory Divide  
215 D7 Keypad +/-  
216 D8 Keypad Clear  
217 D9 Keypad Clear Entry  
218 DA Keypad Binary  
219 DB Keypad Octal  
220 DC Keypad Decimal  
221 DD Keypad Hexadecimal  
222-223 DE-DF Reserved  
224 E0 Keyboard LeftControl 58 √ √ √ 4/101/104  
225 E1 Keyboard LeftShift 44 √ √ √ 4/101/104  
226 E2 Keyboard LeftAlt 60 √ √ √ 4/101/104  
227 E3 Keyboard Left GUI10;23 127 √ √ √ 104  
228 E4 Keyboard RightControl 64 √ √ √ 101/104  
229 E5 Keyboard RightShift 57 √ √ √ 4/101/104  
230 E6 Keyboard RightAlt 62 √ √ √ 101/104  
231 E7 Keyboard Right GUI10;24 128 √ √ √ 104  
232-65535 E8-FFFF Reserved 
*/
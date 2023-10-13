#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aalib.h>
#include "bb.h"
#define MAXLINES 10000
static char *text = "#cWell we've reached the end of this demo. But #bnot#n the #bREAL END#n because\n\n"
"#bAA PROJECT#n is here. \n\n"
"Please read curefully the following document for more information."
"\n"
"\n#b*** This IS the FINAL2 version of BB, modded by stroucki ***#n\n\n"
"#bCHECK OUT http://aa-project.sourceforge.net/#n\n"
"\n"
"First of all\n\n"
"#bControls:#n\n\n"
"f,space,down,j    page down\n"
"b,up,k            page up  \n"
"q,esc             quit     \n"
"1,2,3             song     \n"
"\n#l"
"\n"
"#bContents:#n\n\n"
"o  Advanced usage of this demo\n"
"o  Personal greetings         \n"
"o  AA PROJECT                 \n"
"o  Available programs         \n"
"   o  Aview                   \n"
"   o  XaoS                    \n"
"   o  aaflip                  \n"
"o  The AA-Lib                 \n"
"o  The history                \n"
"#c\n#l\n"
"#c#bAdvanced usage of this demo\n\n"
"           XXXXXXX \n"
"           XX    XX\n"
"XXXXXXX    XX    XX\n"
"XX    XX   XXXXXXX \n"
"XX    XX   XX    XX\n"
"XXXXXXX    XX    XX\n"
"XX    XX   XXXXXXX \n"
"XX    XX           \n"
"XXXXXXX            #n\n"
"\n"
"#c\n"
"#fYou can tune the demo using command line options:\n"
"type bb --help | more   to get all available BB options\n\n"
"We can recommend to you the following combinations:\n\n"
"bb -loop -extended -reverse\n"
"bb -loop -reverse -nobold -nonormal -nodim\n"
"bb -loop -nodim -nobold -extended\n"
"bb -driver stdout -width 79\n"
"bb -driver stdout -width 79 > lpt1 (for some screenshots :)\n\n"
"The option #bfont#n is usefull on hardware where AA-Lib is unable to get current "
"font (like text terminals, mda etc..)\n\n"
"\n"
"Note: the most of this options are common to all #bAA Project#n programs.\n"
"\n#f#l"
"\n"
"#c#bSome BB rumours#n\n"
"\n"
"It was expected to be in SuperVGA mode, truecolor, two CDs\n"
"\n"
"#bPersonal greetings#n\n\n"
"#bHH#n:\n\n"
"#bJaroslav Kysela#n\n"
"I spent three hours patching koules for your drivers..I vote for #bsmaler fragments#n!\n"
"#bMartin Mares#n\n"
"What about searching alg for AAlib?\n"
"#b0rfelyus#n\n"
"be patient..\n"
"#bRoman Zenka\n#n"
"You are in credits again :)\n"
"#bFilip Kroluper#n\n"
"Is YOUR AAlib ready?\n"
"#bRobert Spalek#n\n"
"Lend me your MetafontBook please..\n"
"#bPetr Holicky#n\n"
"I #bLOVE#n your mathematical analyzis lessons!\n"
"\n"
"#bKT#n:\n\n"
"#bReni#n\n"
"All I need is love...\n"
"#bPetr Koloros#n\n"
"What about your songs ?\n"
"#bMartin Koloros#n\n"
"Hey, have you finished Caynis ?\n"
"#bVaclav Tesinsky#n\n"
"I'll join your Delta club\n"
"#bMartin Uddin#n\n"
"Release your adventure game\n"
"#bPetr Smejkal#n\n"
"Still writing those databases, huh ?\n"
"#bMichal Drda#n\n"
"Remove w95 and install Plan9\n"
"#bMichael W. Cerny#n\n"
"Sport!\n"
"#bJaroslav Kysela#n\n"
"Hard coding for your drivers....:)\n"
"\n"
"#bMS#n:\n\n"
"#bDenthor/Asphyxia#n\n"
"yar tutors R the kewlest ones\n"
"#bBLACKAXE#n\n"
"i'll buy U a flask of Absinth !\n"
"#b_MAD#n\n"
"thanq 4 all help\n"
"#bHeadSoft#n\n"
"nice 2 meet ya\n"
"#bSqrt(-1)#n\n"
"hey, dude, thank 4 all ya've done 4 me\n"
"#bQwert#n\n"
"I love you, my dear friend - remember : next year ... ;-)\n"
"#bHeroin#n\n"
"Hey U, Sexual maniac !?\n"
"#bKanec#n\n"
"Don't drink so much !\n"
"#bMarek Les#n\n"
"Hope hear soon your new song !\n"
"#bAnton Szandor LaVey#n\n"
"thanx for The Satanic Bible\n"
"#bMarquis de Sade#n\n"
"big kisses for new style of life\n"
"#bTrent Reznor#n\n"
"the kewlest muzak i've ever heard\n"
"#bJean-Philippe Couasnard#n\n"
"hey, friend .. i hope ya stay here !\n"
"#bMartin Literak#n\n"
"hey, we will fuck them all (but only girls, okay ?)\n"
"\n"
"#bFK#n:\n\n"
"#bEva Stroblova#n\n"
"I want you to want me just like I want you.\n"
"Carry on loving me\n"
"#bWarlord/Scrymag#n\n"
"Haven't you forgotten to close your mouth?\n"
"#bPsycho/Scrymag#n\n"
"Get Psyched!\n"
"#bSmall Electric#n\n"
"Don't you think that 300W sound in your room is too little ?\n"
"#bSystem Trooper/STC#n\n"
"Less Prodigy, more Dawn Music.\n"
"#bM.D.A.#n\n"
"Hey, I'm Samsa incompatible!\n"
"#bVTM/Scrymag#n\n"
"GUS !!!\n"
"#bMamut#n\n"
"No smoking, please.\n"
"#bPurple Motion#n\n"
"Your INSIDE.S3M made me start writting music\n"
"#bJeffrey Lim#n\n"
"Many thanx for Impulse Tracker\n"
"\n"
"\n#l\n"
"#bAA-Project#n\n"
"#c\n"
"#bThree goals of AA-Project:#n"
"\n\n"
"#f1)Port all important software (like Doom, Second Reality, X windows etc..) on new AA-lib.\n"
"\n"
"2)Port AA-lib on all available platforms (mainly ZX-Spectrum and Sharp).\n"
"\n"
"3)Force IBM to start manufacturing MDA cards again.\n"
"\n"
"AA-project was started by Jan Hubicka. In that times just few people knew about \n"
"it. Then a new demo named BB has been relased to show the power \n"
"of AA-lib technology. Now the project is freely available and anyone can help.\n"
"\n"
"Just browse our homepage at #bhttp://aa-project.sourceforge.net/#n. Ton of \n"
"examples of ascii-art generated by aalib, pointers to other AA-Project \n"
"resources etc. \n"
"        \n"
"   We decided to distribute all of our aa-related programs freely (see \n"
"COPYING for details). But if you want to help us develop more such software \n"
"send a small amount (or big one ;-) to AA/BB maintainer address: \n"
"\n"
"Kamil Toman\n"
"CS Armady 1930, Tabor\n"
"390 03\n"
"Czech republic\n"
"\n"
"and contact us at:\n"
"\n"
"kupsaf@feld.cvut.cz (Filip Kupsa)\n"
"hubicka@freesoft.cz (Jan Hubicka)\n"
"titania@horac.ta.jcu.cz (Mojmir Svoboda)\n"
"toman@artax.karlin.mff.cuni.cz (Kamil Toman)\n"
"#f"
"\n"
"#l\n"
"#c#bAvailable programs#n\n"
"#c\n"
"#fThis part contains a listing of programs currently avilable for #bAA-Lib#n. Note that "
"it is probably outdated now. So look at #bhttp://aa-project.sourceforge.net/#n to get latest information"
"#f\n\n\n\n"
"#c#b      dT8  8     dP \n"
"     dT 8  8    dP  \n"
"    dT  8  8   dP   \n"
" <PROJECT> 8  dP    \n"
"  dT    8  8 dP     \n"
" dT     8  8XP Aview\n"
"\n\n#c#f#n"
"This has been historically the first program developed for AA-Lib. In fact it is even older than "
"AA-Lib itself and AA-Lib was later seperated from Aview sources. It was programmed to convert "
"Linux Texas group #bpenguin logo#n to #bascii art#n. It has become powerful #bimage browser#n soon. "
"It is extremly useful in combination of lynx-the best web browser. \n"
"\n"
"Aview has some extended features (including text mode of course) "
"most notably: possibility to #bsave various text formats#n including #bHTML, ANSI "
"terminal escape sequences, Netscapized HTML, pure text file, UNIX more/less format "
"printing to HP-Laser Jet and many others.#n Another useful feature are #bfast image "
"scaling routines#n that automatically scale image to your favorite text resolutions; "
"also controls for all #bAA-Lib features#n are available (including#b contrast, brightness, "
"gamma correction, dithering modes, inversion, font selection, random dithering value#n#f"
"\n"
"\n"
"#c#bXX   XX                 XXXXX \n"
" XX XX   XXXX    XXXX  XX     \n"
"  XXX   XX  XX  XX  XX  XXXXX \n"
" XX XX  XXXXXX  XX  XX      XX\n"
"XX   XX XX  XX   XXXX   XXXXX #n\n"
"#c\n"
"#fXaoS is a #bfast portable realtime interactive fractal zoomer/morpher#n. It was "
"the first program for AA routines in animation style. Current version (2.2) does not "
"contain fully featured support for AA-Lib but it uses its own simplier and less portable "
"routines (it is older than AA-Lib). But AA driver will be available soon after releasing "
"BB. So check out XaoS homepage (http://www.tedium.com/XaoS) for more details "
"#f\n"
"\n"
"#b#cXXXXXX XX      XX   XXXXXX \n"
"XX     XX      XX   XX   XX\n"
"XXXX   XX      XX   XXXXXX \n"
"XX     XX      XX   XX     \n"
"XX     XXXXXXX XX   XX     #n\n"
"#c\n"
"#fFlip is an #bfli/flc player#n originally developed for linux/SVGAlib. A version ported to"
"AA-Lib is available too (called #baaflip#n). It is portable and it works at MS-DOS and "
"probably other architectures too. It supports fullscreen playing of animations, "
"interactive changing of #bbrightness, contrast, gamma correction, value for random dithering."
"#f\n"
"\n#l"
"\n"
"#c#bThe AA-Lib#n\n"
"#f#c\n"
"AA-lib is a low level gfx library just as many other libraries are. The \n"
"main difference is that AA-lib does not require graphics device. In fact, \n"
"there is no graphical output possible. AA-lib replaces those old-fashioned \n"
"output methods with powerful ascii-art renderer. Now my linux boots \n"
"with a nice penguin logo at secondary display (yes! Like Win95 does:) \n"
"AA-lib API is designed to be similar to other graphics libraries. Learning \n"
"a new API would be a piece of cake! \n"
"#f\n"
"\n"
"#l\n"
"#c#bThe history#n\n"
"#f\n"
"Once upon a time we've (my friend Kamil and I) bought two old Herculeses \n"
"as secondary monitors. We didn't know for that time that our Diamond \n"
"Stealths 64 cards would become obsolete soon. Next day we downloaded \n"
"the logo of Linux Texas Users Group \n"
"- nice silly penguin looking like a cowboy! It was so exciting logo ... we \n"
"decided that we couldn't live without it and we wanted to see it every time \n"
"at a boot time as a logo on our secondary monitors. There was a small problem \n"
"- Hercules doesn't support color graphics. So we decided to convert an image \n"
"to ascii art using netpbm tools.\n"
"\n"
"The output was very ugly because the converting algorithm was absolutly stupid. \n"
"During the night I designed a new convertor that used a font bitmap for \n"
"creating aproximation table. The output wasn't very good since the algorithm \n"
"wasn't so well tuned. Many months this small piece of code was waiting on my \n"
"disc for the day \"D\". Meanwhile I started a new project XaoS (a fractal zoomer) \n"
"with my friend Thomas. And then I got an idea: Ascii Art Mandelbrots! \n"
"I was really impressed by the result! XaoS was faster, portable and looking \n"
"much better than ever before. I found a new way to go... "
"#c\n"
"#f\n"
"\n"
"\n"
"\n"
"#b#cAnd remember...\n"
"                    \n"
"      dT8  8Tb      \n"
"     dT 8  8 Tb     \n"
"    dT  8  8  Tb    \n"
" <PROJECT><PROJECT> \n"
"  dT    8  8    Tb  \n"
" dT     8  8     Tb \n"
"\n"
"...means Ascii art!\n"
"#c"
;
#define CENTER 1
#define FILL 2
int mode = 0;
char *line[MAXLINES];
int textsize;
static int currentmode = AA_NORMAL + 1;
static void addline(int width)
{
    int l = textsize;
    line[l] = (char *) malloc(width + 2);
    line[l][0]=AA_NORMAL+1;
    memset(line[l]+1,'~',width);
    line[l][width+1]=0;
    textsize++;
}
static void outputline(char *text, int length1, int length2, int width)
{
    int centerstart = mode & CENTER && !(mode & FILL) ? (width - length1) / 2 : 0;
    int y, y1 = 0, y2;
    int l = textsize;
    int nspaces = 0, wishspaces=0;
    int pspaces=0;
    if(mode&FILL)
    line[l] = (char *) malloc(width + 20); else
    line[l] = (char *) malloc(length1 + centerstart + 20);
    line[l][0] = currentmode;
    for (y = 0; y < centerstart; y++)
	line[l][y + 1] = ' ';
    y1 = centerstart + 1;
    if (mode & FILL) {
	for (y2 = 0; y2 < length2; y2++)
	    if (text[y2] == ' ')
		nspaces++;
	if (nspaces == 0)
	    nspaces = 1;
	wishspaces = width - length1 + nspaces+1;
	if(text[length2]=='\n') wishspaces=nspaces;
    } else
	nspaces = wishspaces = 1;
    for (y2 = y = 0; y2 < length2; y++, y1++, y2++) {
	while (text[y2] == '#') {
	    switch (text[y2 + 1]) {
	    case 'r':
		if (context->driverparams.supported && AA_REVERSE) {
		    currentmode = line[l][y1] = AA_REVERSE + 1;
		    y1++;
		}
		break;
	    case 'n':
		currentmode = line[l][y1] = AA_NORMAL + 1;
		y1++;
		break;
	    case 'd':
		if (context->driverparams.supported && AA_DIM) {
		    currentmode = line[l][y1] = AA_DIM + 1;
		    y1++;
		}
		break;
	    case 'b':
		if (context->driverparams.supported && AA_BOLD) {
		    currentmode = line[l][y1] = AA_BOLD + 1;
		    y1++;
		}
	    }
	    y2 += 2;
	    if(y2>=length2) break;
	}
	if(y2>=length2) break;
	if(text[y2]==' ')
	{int z,m=((pspaces+1)*wishspaces)/nspaces-((pspaces)*wishspaces)/nspaces-1;
	  for(z=0;z<m;z++)
	    line[l][y1] = ' ',y1++;
	  pspaces++;
	}
	line[l][y1] = text[y2];
    }
    line[l][y1] = 0;
    textsize++;
    if(textsize>=MAXLINES) textsize=MAXLINES-1;
}
void format(int width)
{
    int size = strlen(text);
    int i;
    int lastend = 0;
    int lastlength = 0;
    int linelength = 0;
    int linestart = 0;
    for (i = 0; i < size; i++) {
	while (text[i] == '#') {
	    switch (text[i + 1]) {
	    case 'c':
		mode ^= CENTER;
		break;
	    case 'f':
		mode ^= FILL;
		break;
	    case 'l':
	        addline(width);
		break;
	    }
	    i += 2;
	}
	if (text[i] == ' ')
	    lastend = i,lastlength=linelength;
	if (text[i] == '\n')
	    lastend = i,lastlength=linelength;
	if (linelength > width || text[i] == '\n') {
               if(lastend<linestart) lastend=linestart;
	    outputline(text + linestart, lastlength, lastend - linestart, width);
	    linelength = 0;
	    i = lastend;
	    linestart = i + 1;
	}
	linelength++;
    }
    outputline(text + linestart, linelength, i - linestart, width);
}

/*
 * BrlTty - A daemon providing access to the Linux console (when in text
 *          mode) for a blind person using a refreshable braille display.
 *
 * Copyright (C) 1995-2001 by The BrlTty Team. All rights reserved.
 *
 * BrlTty comes with ABSOLUTELY NO WARRANTY.
 *
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation.  Please see the file COPYING for details.
 *
 * Web Page: http://mielke.cc/brltty/
 *
 * This software is maintained by Dave Mielke <dave@mielke.cc>.
 */

/* BrailleNote/brl.c - Braille display library
 * For Pulse Data International's Braille Note series
 * Author: Dave Mielke <dave@mielke.cc>
 */

#define BRL_C

#define __EXTENSIONS__		/* for termios.h */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/termios.h>

#ifdef linux
#include <linux/vt.h>
#endif
static int consoleDescriptor = -1;
static int displayDescriptor = -1;
static int displayTerminal;

#include "../brl.h"
#include "../config.h"
#include "../misc.h"
#include "../inskey.h"
#include "../message.h"
#include "brlconf.h"
#include "../brl_driver.h"

static int fileDescriptor = -1;
static struct termios oldSettings;
static struct termios newSettings;

static unsigned char *cellBuffer = NULL;
static unsigned int cellCount = 0;
static unsigned char *statusArea;
static unsigned int statusCells;
static unsigned char *dataArea;
static unsigned int dataCells;
static unsigned char *outputBuffer = NULL;

typedef enum {
   KBM_INPUT,
   KBM_INPUT_7,
   KBM_INPUT_78,
   KBM_INPUT_8,
   KBM_NAVIGATE
} KeyboardMode;
static KeyboardMode persistentKeyboardMode;
static KeyboardMode temporaryKeyboardMode;
static KeyboardMode currentKeyboardMode;

static unsigned int persistentRoutingOperation;
static unsigned int temporaryRoutingOperation;
static unsigned int currentRoutingOperation;

const unsigned char outputTable[] = {
   0X00, 0X01, 0X08, 0X09, 0X02, 0X03, 0X0A, 0X0B,
   0X10, 0X11, 0X18, 0X19, 0X12, 0X13, 0X1A, 0X1B,
   0X04, 0X05, 0X0C, 0X0D, 0X06, 0X07, 0X0E, 0X0F,
   0X14, 0X15, 0X1C, 0X1D, 0X16, 0X17, 0X1E, 0X1F,
   0X20, 0X21, 0X28, 0X29, 0X22, 0X23, 0X2A, 0X2B,
   0X30, 0X31, 0X38, 0X39, 0X32, 0X33, 0X3A, 0X3B,
   0X24, 0X25, 0X2C, 0X2D, 0X26, 0X27, 0X2E, 0X2F,
   0X34, 0X35, 0X3C, 0X3D, 0X36, 0X37, 0X3E, 0X3F,
   0X40, 0X41, 0X48, 0X49, 0X42, 0X43, 0X4A, 0X4B,
   0X50, 0X51, 0X58, 0X59, 0X52, 0X53, 0X5A, 0X5B,
   0X44, 0X45, 0X4C, 0X4D, 0X46, 0X47, 0X4E, 0X4F,
   0X54, 0X55, 0X5C, 0X5D, 0X56, 0X57, 0X5E, 0X5F,
   0X60, 0X61, 0X68, 0X69, 0X62, 0X63, 0X6A, 0X6B,
   0X70, 0X71, 0X78, 0X79, 0X72, 0X73, 0X7A, 0X7B,
   0X64, 0X65, 0X6C, 0X6D, 0X66, 0X67, 0X6E, 0X6F,
   0X74, 0X75, 0X7C, 0X7D, 0X76, 0X77, 0X7E, 0X7F,
   0X80, 0X81, 0X88, 0X89, 0X82, 0X83, 0X8A, 0X8B,
   0X90, 0X91, 0X98, 0X99, 0X92, 0X93, 0X9A, 0X9B,
   0X84, 0X85, 0X8C, 0X8D, 0X86, 0X87, 0X8E, 0X8F,
   0X94, 0X95, 0X9C, 0X9D, 0X96, 0X97, 0X9E, 0X9F,
   0XA0, 0XA1, 0XA8, 0XA9, 0XA2, 0XA3, 0XAA, 0XAB,
   0XB0, 0XB1, 0XB8, 0XB9, 0XB2, 0XB3, 0XBA, 0XBB,
   0XA4, 0XA5, 0XAC, 0XAD, 0XA6, 0XA7, 0XAE, 0XAF,
   0XB4, 0XB5, 0XBC, 0XBD, 0XB6, 0XB7, 0XBE, 0XBF,
   0XC0, 0XC1, 0XC8, 0XC9, 0XC2, 0XC3, 0XCA, 0XCB,
   0XD0, 0XD1, 0XD8, 0XD9, 0XD2, 0XD3, 0XDA, 0XDB,
   0XC4, 0XC5, 0XCC, 0XCD, 0XC6, 0XC7, 0XCE, 0XCF,
   0XD4, 0XD5, 0XDC, 0XDD, 0XD6, 0XD7, 0XDE, 0XDF,
   0XE0, 0XE1, 0XE8, 0XE9, 0XE2, 0XE3, 0XEA, 0XEB,
   0XF0, 0XF1, 0XF8, 0XF9, 0XF2, 0XF3, 0XFA, 0XFB,
   0XE4, 0XE5, 0XEC, 0XED, 0XE6, 0XE7, 0XEE, 0XEF,
   0XF4, 0XF5, 0XFC, 0XFD, 0XF6, 0XF7, 0XFE, 0XFF
};

const unsigned char inputTable[] = {
   0X00, 0X01, 0X04, 0X05, 0X10, 0X11, 0X14, 0X15,
   0X02, 0X03, 0X06, 0X07, 0X12, 0X13, 0X16, 0X17,
   0X08, 0X09, 0X0C, 0X0D, 0X18, 0X19, 0X1C, 0X1D,
   0X0A, 0X0B, 0X0E, 0X0F, 0X1A, 0X1B, 0X1E, 0X1F,
   0X20, 0X21, 0X24, 0X25, 0X30, 0X31, 0X34, 0X35,
   0X22, 0X23, 0X26, 0X27, 0X32, 0X33, 0X36, 0X37,
   0X28, 0X29, 0X2C, 0X2D, 0X38, 0X39, 0X3C, 0X3D,
   0X2A, 0X2B, 0X2E, 0X2F, 0X3A, 0X3B, 0X3E, 0X3F,
   0X40, 0X41, 0X44, 0X45, 0X50, 0X51, 0X54, 0X55,
   0X42, 0X43, 0X46, 0X47, 0X52, 0X53, 0X56, 0X57,
   0X48, 0X49, 0X4C, 0X4D, 0X58, 0X59, 0X5C, 0X5D,
   0X4A, 0X4B, 0X4E, 0X4F, 0X5A, 0X5B, 0X5E, 0X5F,
   0X60, 0X61, 0X64, 0X65, 0X70, 0X71, 0X74, 0X75,
   0X62, 0X63, 0X66, 0X67, 0X72, 0X73, 0X76, 0X77,
   0X68, 0X69, 0X6C, 0X6D, 0X78, 0X79, 0X7C, 0X7D,
   0X6A, 0X6B, 0X6E, 0X6F, 0X7A, 0X7B, 0X7E, 0X7F,
   0X80, 0X81, 0X84, 0X85, 0X90, 0X91, 0X94, 0X95,
   0X82, 0X83, 0X86, 0X87, 0X92, 0X93, 0X96, 0X97,
   0X88, 0X89, 0X8C, 0X8D, 0X98, 0X99, 0X9C, 0X9D,
   0X8A, 0X8B, 0X8E, 0X8F, 0X9A, 0X9B, 0X9E, 0X9F,
   0XA0, 0XA1, 0XA4, 0XA5, 0XB0, 0XB1, 0XB4, 0XB5,
   0XA2, 0XA3, 0XA6, 0XA7, 0XB2, 0XB3, 0XB6, 0XB7,
   0XA8, 0XA9, 0XAC, 0XAD, 0XB8, 0XB9, 0XBC, 0XBD,
   0XAA, 0XAB, 0XAE, 0XAF, 0XBA, 0XBB, 0XBE, 0XBF,
   0XC0, 0XC1, 0XC4, 0XC5, 0XD0, 0XD1, 0XD4, 0XD5,
   0XC2, 0XC3, 0XC6, 0XC7, 0XD2, 0XD3, 0XD6, 0XD7,
   0XC8, 0XC9, 0XCC, 0XCD, 0XD8, 0XD9, 0XDC, 0XDD,
   0XCA, 0XCB, 0XCE, 0XCF, 0XDA, 0XDB, 0XDE, 0XDF,
   0XE0, 0XE1, 0XE4, 0XE5, 0XF0, 0XF1, 0XF4, 0XF5,
   0XE2, 0XE3, 0XE6, 0XE7, 0XF2, 0XF3, 0XF6, 0XF7,
   0XE8, 0XE9, 0XEC, 0XED, 0XF8, 0XF9, 0XFC, 0XFD,
   0XEA, 0XEB, 0XEE, 0XEF, 0XFA, 0XFB, 0XFE, 0XFF
};

static void
systemError (const char *action)
{
   LogPrint(LOG_ERR, "%s error %d: %s.", action, errno, strerror(errno));
}

static void
refreshCells (void)
{
   unsigned char *output = outputBuffer;
   int i;

   *output++ = BNO_BEGIN;
   *output++ = BNO_WRITE;

   for (i=0; i<cellCount; ++i) {
      if ((*output++ = outputTable[cellBuffer[i]]) == BNO_BEGIN) {
         *output++ = BNO_BEGIN;
      }
   }

   safe_write(fileDescriptor, outputBuffer, output-outputBuffer);
}

static void
writePrompt(const unsigned char *prompt)
{
   unsigned int length = strlen(prompt);
   unsigned int index = 0;
   if (length > dataCells) {
      length = dataCells;
   }
   while (index < length) {
      dataArea[index] = texttrans[prompt[index]];
      ++index;
   }
   while (index < dataCells) {
      dataArea[index++] = 0;
   }
   refreshCells();
}

static unsigned char
getByte(void)
{
   unsigned char byte;
   fd_set mask;
   FD_ZERO(&mask);
   FD_SET(fileDescriptor, &mask);
   select(fileDescriptor+1, &mask, NULL, NULL, NULL);
   read(fileDescriptor, &byte, 1);
   return byte;
}

static unsigned char
getCharacter(void)
{
   for (;;) {
      switch (getByte()) {
         default:
	    break;
	 case BNI_CHARACTER:
	    return untexttrans[inputTable[getByte()]];
	 case BNI_SPACE:
	    switch (getByte()) {
	       default:
	          break;
	       case BNC_SPACE:
		  return ' ';
	    }
	    break;
	 case BNI_BACKSPACE:
	    switch (getByte() & 0X3F) {
	       default:
	          break;
	       case BNC_SPACE:
		  return '\b';
	    }
	    break;
	 case BNI_ENTER:
	    switch (getByte()) {
	       default:
	          break;
	       case BNC_SPACE:
		  return '\r';
	    }
	    break;
      }
      refreshCells();
   }
}

static void
openConsole(void)
{
   if (consoleDescriptor == -1) {
      const char *consoleDevice = "/dev/console";
      consoleDescriptor = open(consoleDevice, O_RDONLY);
   }
}

static int
getVirtualTerminal(void)
{
   int vt = -1;
#ifdef VT_GETSTATE
   openConsole();
   if (consoleDescriptor != -1) {
      struct vt_stat state;
      if (ioctl(consoleDescriptor, VT_GETSTATE, &state) != -1) {
         vt = state.v_active;
      }
   }
#endif
   return vt;
}

static void
setVirtualTerminal(int vt)
{
#ifdef VT_ACTIVATE
   openConsole();
   if (consoleDescriptor != -1) {
      LogPrint(LOG_DEBUG, "switching to virtual terminal %d", vt);
      if (ioctl(consoleDescriptor, VT_ACTIVATE, vt) != -1) {
	 if (ioctl(consoleDescriptor, VT_WAITACTIVE, vt) != -1) {
	    LogPrint(LOG_INFO, "switched to virtual terminal %d", vt);
	 } else {
	    systemError("virtual console wait");
	 }
      } else {
         systemError("virtual console activate");
      }
   }
#endif
}

static void
openVisualDisplay(void)
{
#ifdef VT_OPENQRY
   if (displayDescriptor == -1) {
      openConsole();
      if (consoleDescriptor != -1) {
	 if (ioctl(consoleDescriptor, VT_OPENQRY, &displayTerminal) != -1) {
	    char path[0X20];
	    snprintf(path, sizeof(path), "/dev/tty%d", displayTerminal);
	    if ((displayDescriptor = open(path, O_WRONLY)) != -1) {
	       LogPrint(LOG_INFO, "visual display is %s", path);
	    }
	 }
      }
   }
   if (displayDescriptor != -1) {
      setVirtualTerminal(displayTerminal);
   }
#endif
}

static void
closeVisualDisplay(int vt)
{
   if (displayDescriptor != -1) {
      if (getVirtualTerminal() == displayTerminal) {
         setVirtualTerminal(vt);
      }
      close(displayDescriptor);
      displayDescriptor = -1;
      displayTerminal = 0;
   }
}

static void
writeVisualDisplay(unsigned char c)
{
   if (displayDescriptor != -1) {
      write(displayDescriptor, &c, 1);
   }
}

static int
visualDisplay(unsigned char character)
{
   int vt = getVirtualTerminal();
   const unsigned char end[] = {0X1B, 0};
   unsigned int state = 0;
   openVisualDisplay();
   writeVisualDisplay(BNI_DISPLAY);
   writeVisualDisplay(character);
   for (;;) {
      character = getByte();
      if (character == end[state]) {
	 if (++state == sizeof(end)) {
	    break;
	 }
      } else {
	 if (state > 0) {
	    int i;
	    for (i=0; i<state; ++i) {
	       writeVisualDisplay(end[i]);
	    }
	    state = 0;
	 }
	 if (character == end[0]) {
	    state = 1;
	 } else {
	    writeVisualDisplay(character);
	 }
      }
   }
   closeVisualDisplay(vt);
   return EOF;
}

static void
identbrl (void)
{
   LogPrint(LOG_NOTICE, "BrailleNote driver, version 1.0");
   LogPrint(LOG_INFO, "   Copyright (C) 2001 by Dave Mielke <dave@mielke.cc>");
}

static void
initbrl (brldim *brl, const char *device)
{
   if ((fileDescriptor = open(device, O_RDWR|O_NOCTTY|O_NONBLOCK)) != -1) {
      if (tcgetattr(fileDescriptor, &oldSettings) != -1) {
	 memset(&newSettings, 0, sizeof(newSettings));
	 newSettings.c_cflag = CS8 | CSTOPB | CLOCAL | CREAD;
	 newSettings.c_iflag = IGNPAR;
	 cfsetispeed(&newSettings, B38400);
	 cfsetospeed(&newSettings, B38400);
	 if (tcsetattr(fileDescriptor, TCSANOW, &newSettings) != -1) {
	    if (tcflush(fileDescriptor, TCIOFLUSH) != -1) {
	       unsigned char request[] = {BNO_BEGIN, BNO_DESCRIBE};
	       if (safe_write(fileDescriptor, request, sizeof(request)) != -1) {
		  unsigned char response[3];
		  int count;
		  delay(500);
		  if ((count = safe_read(fileDescriptor, response, sizeof(response))) != -1) {
		     if (response[0] == 0X86) {
			statusCells = response[1];
			brl->x = response[2];
			brl->y = 1;
			if ((statusCells == 5) && (brl->x == 30)) {
			   statusCells = 0;
			   brl->x += 2;
			}
			dataCells = brl->x * brl->y;
			cellCount = statusCells + dataCells;
			if ((cellBuffer = malloc(cellCount))) {
			   memset(cellBuffer, 0, cellCount);
			   statusArea = cellBuffer;
			   dataArea = statusArea + statusCells;
			   if ((outputBuffer = malloc((cellCount * 2) + 2))) {
			      if ((brl->disp = malloc(dataCells))) {
				 memset(brl->disp, 0, dataCells);
				 refreshCells();
				 persistentKeyboardMode = KBM_NAVIGATE;
				 temporaryKeyboardMode = persistentKeyboardMode;
				 persistentRoutingOperation = CR_ROUTEOFFSET;
				 temporaryRoutingOperation = persistentRoutingOperation;
				 return;
			      }
			      free(outputBuffer);
			   } else {
			      systemError("output buffer allocation");
			   }
			   free(cellBuffer);
			} else {
			   systemError("cell buffer allocation");
			}
		     } else {
			LogPrint(LOG_ERR, "unexpected response: %2.2X %2.2X %2.2X",
				     response[0], response[1], response[2]);
		     }
		  } else {
		     systemError("device read");
		  }
	       } else {
		  systemError("device write");
	       }
	    } else {
	       systemError("device flush");
	    }
	    tcsetattr(fileDescriptor, TCSANOW, &oldSettings);
	 } else {
	    systemError("device attribute set");
	 }
      } else {
	 systemError("device attribute query");
      }
      close(fileDescriptor);
   } else {
      systemError("device open");
   }
   brl->x = -1;
   brl->y = -1;
}

static void
closebrl (brldim *brl)
{
   free(brl->disp);
   free(outputBuffer);
   free(cellBuffer);
   tcflush(fileDescriptor,TCIOFLUSH);
   tcsetattr(fileDescriptor, TCSANOW, &oldSettings);
   close(fileDescriptor);
}

static void
writebrl (brldim *brl)
{
   if (memcmp(dataArea, brl->disp, dataCells) != 0) {
      memcpy(dataArea, brl->disp, dataCells);
      refreshCells();
   }
}

static void
setbrlstat (const unsigned char *status)
{
   if (memcmp(statusArea, status, statusCells) != 0) {
      memcpy(statusArea, status, statusCells);
      refreshCells();
   }
}

static int
getDecimalInteger(unsigned int *integer, unsigned int width, const char *description)
{
   char buffer[width + 1];
   memset(buffer, '0', width);
   buffer[width] = 0;
   for (;;) {
      unsigned char character;
      char prompt[0X40];
      snprintf(prompt, sizeof(prompt), "%s: %s", description, buffer);
      writePrompt(prompt);
      switch (character = getCharacter()) {
         default:
	    continue;
	 case '\r':
	    *integer = atoi(buffer);
	    return 1;
	 case '\b':
	    return 0;
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
	    memcpy(buffer, buffer+1, width-1);
	    buffer[width-1] = character;
	    break;
      }
   }
}

static int
getHexadecimalCharacter(unsigned char *character)
{
   *character = 0X00;
   for (;;) {
      unsigned char digit;
      char prompt[0X40];
      snprintf(prompt, sizeof(prompt), "hex char: %2.2x %c", *character, *character);
      writePrompt(prompt);
      switch (getCharacter()) {
         default:
	    continue;
	 case '\r':
	    return 1;
	 case '\b':
	    return 0;
         case '0':
	    digit = 0X0;
	    break;
         case '1':
	    digit = 0X1;
	    break;
         case '2':
	    digit = 0X2;
	    break;
         case '3':
	    digit = 0X3;
	    break;
         case '4':
	    digit = 0X4;
	    break;
         case '5':
	    digit = 0X5;
	    break;
         case '6':
	    digit = 0X6;
	    break;
         case '7':
	    digit = 0X7;
	    break;
         case '8':
	    digit = 0X8;
	    break;
         case '9':
	    digit = 0X9;
	    break;
         case 'a':
	    digit = 0XA;
	    break;
         case 'b':
	    digit = 0XB;
	    break;
         case 'c':
	    digit = 0XC;
	    break;
         case 'd':
	    digit = 0XD;
	    break;
         case 'e':
	    digit = 0XE;
	    break;
         case 'f':
	    digit = 0XF;
	    break;
      }
      *character = (*character << 4) | digit;
   }
}

static void
enterFunctionKey(void)
{
   const char *keyTable[] = {
      KEY_F1 , KEY_F2 , KEY_F3 , KEY_F4 ,
      KEY_F5 , KEY_F6 , KEY_F7 , KEY_F8 ,
      KEY_F9 , KEY_F10, KEY_F11, KEY_F12,
      KEY_F13, KEY_F14, KEY_F15, KEY_F16,
      KEY_F17, KEY_F18, KEY_F19, KEY_F20
   };
   const unsigned int keyCount = sizeof(keyTable) / sizeof(keyTable[0]);
   unsigned int keyNumber;
   if (getDecimalInteger(&keyNumber, 2, "function key")) {
      if ((keyNumber > 0) && (keyNumber <= keyCount)) {
         inskey(keyTable[keyNumber-1]);
      }
   }
}

static int
interpretNavigation(unsigned char dots)
{
   switch (dots) {
      default:
         break;
      case BND_0:
         return CMD_HOME;
      case (BND_1):
	 return CMD_CHRLT;
      case (BND_1 | BND_2):
	 return CMD_HWINLT;
      case (BND_2):
	 return CMD_FWINLT;
      case (BND_2 | BND_3):
	 return CMD_FWINLTSKIP;
      case (BND_3):
	 return CMD_LNBEG;
      case (BND_1 | BND_3):
	 return CMD_LNUP;
      case (BND_1 | BND_2 | BND_3):
	 return CMD_TOP_LEFT;
      case (BND_4):
	 return CMD_CHRRT;
      case (BND_4 | BND_5):
	 return CMD_HWINRT;
      case (BND_5):
	 return CMD_FWINRT;
      case (BND_5 | BND_6):
	 return CMD_FWINRTSKIP;
      case (BND_6):
	 return CMD_LNEND;
      case (BND_4 | BND_6):
	 return CMD_LNDN;
      case (BND_4 | BND_5 | BND_6):
	 return CMD_BOT_LEFT;
      case (BND_1 | BND_4):
	 return CMD_TOP;
      case (BND_2 | BND_5):
	 return CMD_HOME;
      case (BND_3 | BND_6):
	 return CMD_BOT;
      case (BND_1 | BND_4 | BND_5):
	 return CMD_PRDIFLN;
      case (BND_2 | BND_5 | BND_6):
	 return CMD_NXDIFLN;
      case (BND_1 | BND_2 | BND_4):
	 return CMD_PRSEARCH;
      case (BND_2 | BND_3 | BND_5):
	 return CMD_NXSEARCH;
      case (BND_1 | BND_2 | BND_5):
	 return CMD_ATTRUP;
      case (BND_2 | BND_3 | BND_6):
	 return CMD_ATTRDN;
      case (BND_2 | BND_4):
	 temporaryRoutingOperation = CR_PRINDENT;
	 return CMD_NOOP;
      case (BND_3 | BND_5):
	 temporaryRoutingOperation = CR_NXINDENT;
	 return CMD_NOOP;
      case (BND_2 | BND_4 | BND_5):
	 return CMD_WINUP;
      case (BND_3 | BND_5 | BND_6):
	 return CMD_WINDN;
   }
   return EOF;
}

static int
interpretCharacter(unsigned char dots)
{
   int value = inputTable[dots];
   switch (currentKeyboardMode) {
      case KBM_NAVIGATE:
         return interpretNavigation(dots);
      case KBM_INPUT:
	 break;
      case KBM_INPUT_7:
	 value |= 0X40;
	 break;
      case KBM_INPUT_78:
	 value |= 0XC0;
	 break;
      case KBM_INPUT_8:
	 value |= 0X80;
	 break;
   }
   return VAL_BRLKEY + value;
}

static int
interpretSpaceChord(unsigned char dots)
{
   switch (dots) {
      default:
      // These are overridden by the Braille Note itself.
      case BNC_E: // exit current operation
      case BNC_H: // help for current operation
      case BNC_O: // go to options menu
      case BNC_R: // repeat current prompt
      case BNC_U: // uppercase for computer braille
      case BNC_Z: // exit current operation
      case BNC_6: // go to task menu
      case (BND_1 | BND_2 | BND_3 | BND_4 | BND_5 | BND_6): // go to main menu
	 break;
      case BNC_SPACE:
	 return interpretCharacter(dots);
      case BNC_C:
	 return CMD_CONFMENU;
      case BNC_D:
	 return CMD_RESET;
      case BNC_F:
	 enterFunctionKey();
         return EOF;
      case BNC_L:
	 return CMD_CSRJMP_VERT;
      case BNC_M:
	 return CMD_MUTE;
      case BNC_N:
         persistentKeyboardMode = KBM_NAVIGATE;
	 temporaryKeyboardMode = persistentKeyboardMode;
	 return CMD_NOOP;
      case BNC_P:
	 return CMD_PASTE;
      case BNC_S:
	 return CMD_SAY;
      case BNC_V: {
	 unsigned int vt;
	 if (getDecimalInteger(&vt, 2, "virt term num")) {
	    if ((vt > 0) && (vt <= 0x40)) {
	       return CR_SWITCHVT + (vt - 1);
	    }
	 }
         return EOF;
      }
      case BNC_W:
	 return CMD_SAVECONF;
      case BNC_X: {
	 unsigned char character;
	 if (!getHexadecimalCharacter(&character)) {
	    return EOF;
	 }
	 return VAL_PASSTHRU + character;
      }
      case BNC_LPAREN:
	 temporaryRoutingOperation = CR_BEGBLKOFFSET;
	 return CMD_NOOP;
      case BNC_RPAREN:
	 temporaryRoutingOperation = CR_ENDBLKOFFSET;
	 return CMD_NOOP;
      case (BND_2 | BND_3 | BND_5 | BND_6):
	 inskey(KEY_TAB);
	 return EOF;
      case (BND_2 | BND_3):
	 inskey(LT_CSR);
	 return EOF;
      case (BND_5 | BND_6):
	 inskey(RT_CSR);
	 return EOF;
      case (BND_2 | BND_5):
	 inskey(UP_CSR);
	 return EOF;
      case (BND_3 | BND_6):
	 inskey(DN_CSR);
	 return EOF;
      case (BND_2):
	 inskey(KEY_HOME);
	 return EOF;
      case (BND_3):
	 inskey(KEY_END);
	 return EOF;
      case (BND_5):
	 inskey(UP_PAGE);
	 return EOF;
      case (BND_6):
	 inskey(DN_PAGE);
	 return EOF;
      case (BND_3 | BND_5):
	 inskey(KEY_INSERT);
	 return EOF;
      case (BND_2 | BND_5 | BND_6):
	 inskey(KEY_DELETE);
	 return EOF;
      case (BND_2 | BND_6):
	 inskey(KEY_ESCAPE);
	 return EOF;
      case (BND_4):
      case (BND_4 | BND_5):
         temporaryKeyboardMode = KBM_INPUT;
	 goto kbmFinish;
      case (BND_4 | BND_3):
      case (BND_4 | BND_5 | BND_3):
         temporaryKeyboardMode = KBM_INPUT_7;
	 goto kbmFinish;
      case (BND_4 | BND_3 | BND_6):
      case (BND_4 | BND_5 | BND_3 | BND_6):
         temporaryKeyboardMode = KBM_INPUT_78;
	 goto kbmFinish;
      case (BND_4 | BND_6):
      case (BND_4 | BND_5 | BND_6):
         temporaryKeyboardMode = KBM_INPUT_8;
      kbmFinish:
	 if (dots & BND_5) {
	    persistentKeyboardMode = temporaryKeyboardMode;
	 }
	 return CMD_NOOP;
   }
   return EOF;
}

static int
interpretBackspaceChord(unsigned char dots)
{
   switch (dots & 0X3F) {
      default:
	 break;
      case BNC_SPACE:
	 inskey(KEY_BACKSPACE);
	 return EOF;
      case BNC_A:
	 return CMD_DISPMD | VAL_SWITCHON;
      case BNC_H:
	 return CMD_HELP;
      case BNC_S:
	 return CMD_INFO;
      case BNC_T:
	 return CMD_DISPMD | VAL_SWITCHOFF;
      case BNC_6:
         return CMD_SIXDOTS | VAL_SWITCHON;
      case BNC_8:
         return CMD_SIXDOTS | VAL_SWITCHOFF;
      case (BND_1 | BND_2 | BND_3 | BND_4 | BND_5 | BND_6):
	 return CMD_RESTARTSPEECH;
   }
   return EOF;
}

static int
interpretEnterChord(unsigned char dots)
{
   switch (dots) {
      default:
      // These are overridden by the Braille Note itself.
      case (BND_1): // decrease speech volume
      case (BND_4): // increase speech volume
      case (BND_2): // decrease speech pitch
      case (BND_5): // increase speech pitch
      case (BND_3): // decrease speech speed
      case (BND_6): // increase speech speed
      case BNC_D: // display the date
      case BNC_H: // hear punctuation in current prompt
      case BNC_S: // spell name in current prompt
      case BNC_T: // display the time
	 break;
      case BNC_SPACE:
	 inskey(KEY_RETURN);
	 return EOF;
      case BNC_B:
         return CMD_SKPBLNKWINS | VAL_SWITCHON;
      case BNC_F:
         return CMD_FREEZE | VAL_SWITCHON;
      case BNC_I:
         return CMD_SKPIDLNS | VAL_SWITCHON;
      case BNC_W:
         return CMD_SLIDEWIN | VAL_SWITCHON;
      case (BND_1 | BND_2 | BND_3 | BND_4 | BND_5 | BND_6):
	 return CMD_RESTARTBRL;
   }
   return EOF;
}

static int
interpretThumbKeys(unsigned char keys)
{
   switch (keys) {
      default:
	 break;
      case (BNT_PREVIOUS):
	 return CMD_FWINLT;
      case (BNT_NEXT):
	 return CMD_FWINRT;
      case (BNT_BACK):
	 return CMD_LNUP;
      case (BNT_ADVANCE):
	 return CMD_LNDN;
      case (BNT_PREVIOUS | BNT_BACK):
	 return CMD_LNBEG;
      case (BNT_NEXT | BNT_ADVANCE):
	 return CMD_LNEND;
      case (BNT_PREVIOUS | BNT_ADVANCE):
	 return CMD_TOP_LEFT;
      case (BNT_PREVIOUS | BNT_NEXT):
	 return CMD_BOT_LEFT;
      case (BNT_BACK | BNT_ADVANCE):
	 return CMD_CSRTRK;
      case (BNT_BACK | BNT_NEXT):
	 return CMD_CSRJMP_VERT;
   }
   return EOF;
}

static int
interpretRoutingKey(unsigned char key)
{
   return currentRoutingOperation + key;
}

static int
readbrl (int type)
{
   unsigned char character;
   int (*handler)(unsigned char);

   if (read(fileDescriptor, &character, 1) != 1) {
      return EOF;
   }
   switch (character) {
      default:
         return EOF;
      case BNI_CHARACTER:
         handler = interpretCharacter;
	 break;
      case BNI_SPACE:
         handler = interpretSpaceChord;
	 break;
      case BNI_BACKSPACE:
         handler = interpretBackspaceChord;
	 break;
      case BNI_ENTER:
         handler = interpretEnterChord;
	 break;
      case BNI_THUMB:
         handler = interpretThumbKeys;
	 break;
      case BNI_ROUTE:
         handler = interpretRoutingKey;
	 break;
      case BNI_DISPLAY:
         handler = visualDisplay;
	 break;
   }

   delay(1);
   if (read(fileDescriptor, &character, 1) != 1) {
      return EOF;
   }

   currentKeyboardMode = temporaryKeyboardMode;
   temporaryKeyboardMode = persistentKeyboardMode;

   currentRoutingOperation = temporaryRoutingOperation;
   temporaryRoutingOperation = persistentRoutingOperation;

   return handler(character);
}

/* main.c - this file is part of DeSmuME
 *
 * Copyright (C) 2007 Damien Nozay (damdoum)
 * Author: damdoum at users.sourceforge.net
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "callbacks.h"
#include "callbacks_IO.h"
#include "dTools/callbacks_dtools.h"
#include "globals.h"

GtkWidget * pWindow;
GtkWidget * pDrawingArea;
GtkWidget * pDrawingArea2;
GladeXML  * xml, * xml_tools;

SoundInterface_struct *SNDCoreList[] = {
&SNDDummy,
&SNDFile,
&SNDSDL,
NULL
};

/* ***** ***** CONFIG FILE ***** ***** */

gint Keypad_Config[DESMUME_NB_KEYS];

const u16 Default_Joypad_Config[DESMUME_NB_KEYS] =
  { 1,  // A
    0,  // B
    5,  // select
    8,  // start
    20, // Right -- Start cheating abit...
    21, // Left
    22, // Up
    23, // Down  -- End of cheating.
    7,  // R
    6,  // L
    3,  // Y
    4,  // X
    -1, // DEBUG
    -1  // BOOST
  };

const char *Ini_Keypad_Values[DESMUME_NB_KEYS] =
{
	"KEY_A",
	"KEY_B",
	"KEY_SELECT",
	"KEY_START",
	"KEY_RIGHT",
	"KEY_LEFT",
	"KEY_UP",
	"KEY_DOWN",
	"KEY_R",
	"KEY_L",
	"KEY_Y",
	"KEY_X",
	"KEY_DEBUG",
	"KEY_BOOST"
};

const gint Default_Keypad_Config[DESMUME_NB_KEYS] =
{
	GDK_b,
	GDK_a,
	GDK_BackSpace,
	GDK_Return,
	GDK_Right,
	GDK_Left,
	GDK_Up,
	GDK_Down,
	GDK_KP_Decimal,
	GDK_KP_0,
	GDK_y,
	GDK_x,
	GDK_p,
	GDK_o
};

char * CONFIG_FILE;

void inline Load_DefaultConfig()
{
	memcpy(Keypad_Config, Default_Keypad_Config, sizeof(Keypad_Config));
}

int Read_ConfigFile()
{
	int i, tmp;
	GKeyFile * keyfile = g_key_file_new();
	GError * error = NULL;
	
	Load_DefaultConfig();
	
	g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, 0);

	const char *c;
		
	for(i = 0; i < DESMUME_NB_KEYS; i++)
	{
		tmp = g_key_file_get_integer(keyfile, "KEYS", Ini_Keypad_Values[i], &error);
		if (error != NULL) {
			g_error_free(error);
			error = NULL;
		} else {
			Keypad_Config[i] = g_key_file_get_integer(keyfile, "KEYS", Ini_Keypad_Values[i], &error);
		}
	}
		
	g_key_file_free(keyfile);
		
	return 0;
}

int Write_ConfigFile()
{
	int i;
	GKeyFile * keyfile;
	
	keyfile = g_key_file_new();
	
	for(i = 0; i < DESMUME_NB_KEYS; i++)
	{
		g_key_file_set_integer(keyfile, "KEYS", Ini_Keypad_Values[i], Keypad_Config[i]);
	}
	
	g_file_set_contents(CONFIG_FILE, g_key_file_to_data(keyfile, 0, 0), -1, 0);
	g_key_file_free(keyfile);
	
	return 0;
}






/* ***** ***** MAIN ***** ***** */


#ifdef WIN32
int WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
	main(0,NULL);
}
#endif

int main(int argc, char *argv[]) {
	
	const char *commandLine_File = NULL;
	int nbJoysticks;
        int i;

	if(argc == 2) commandLine_File = argv[1];

#ifdef DEBUG
        LogStart();
#endif
	init_keyvals();
        
	memcpy(Joypad_Config, Default_Joypad_Config, sizeof(Joypad_Config));

	gtk_init(&argc, &argv);
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
          {
            fprintf(stderr, "Error trying to initialize SDL: %s\n",
                    SDL_GetError());
            return 1;
          }
	desmume_init();

	CONFIG_FILE = g_build_filename(g_get_home_dir(), ".desmume.ini", NULL);
	Read_ConfigFile();

        /* Initialize joysticks */
        nbJoysticks = SDL_NumJoysticks();
        printf("Nbr of joysticks: %d\n\n", nbJoysticks);

        for (i = 0; i < nbJoysticks; i++)
          {
            SDL_Joystick * joy = SDL_JoystickOpen(i);
            printf("Joystick %s\n", i, SDL_JoystickName(i));
            printf("Axes: %d\n", SDL_JoystickNumAxes(joy));
            printf("Buttons: %d\n", SDL_JoystickNumButtons(joy));
            printf("Trackballs: %d\n", SDL_JoystickNumBalls(joy));
            printf("Hats: %d\n\n", SDL_JoystickNumHats(joy));
          }

	/* load the interface */
	xml           = glade_xml_new("glade/DeSmuMe.glade", NULL, NULL);
	xml_tools     = glade_xml_new("glade/DeSmuMe_Dtools.glade", NULL, NULL);
	pWindow       = glade_xml_get_widget(xml, "wMainW");
	pDrawingArea  = glade_xml_get_widget(xml, "wDraw_Main");
	pDrawingArea2 = glade_xml_get_widget(xml, "wDraw_Sub");
	
	/* connect the signals in the interface */
	glade_xml_signal_autoconnect(xml);
	glade_xml_signal_autoconnect(xml_tools);

	/* Vérifie la ligne de commandes */
	if(commandLine_File) {
		if(desmume_open(commandLine_File) >= 0)	{
			desmume_resume();
			enable_rom_features();
		} else {
			GtkWidget *pDialog = gtk_message_dialog_new(GTK_WINDOW(pWindow),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_INFO,
					GTK_BUTTONS_OK,
					"Unable to load :\n%s", commandLine_File);
			gtk_dialog_run(GTK_DIALOG(pDialog));
			gtk_widget_destroy(pDialog);
		}
	}

	/* start event loop */
	gtk_main();
	desmume_free();

#ifdef DEBUG
        LogStop();
#endif

	SDL_Quit();
	Write_ConfigFile();
	return EXIT_SUCCESS;
}


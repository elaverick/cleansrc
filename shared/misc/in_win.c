/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// in_win.c -- windows 95 mouse code

#include		"quakedef.h"
#include		"winquake.h"

POINT			current_pos;

int				mouse_buttons;
int				mouse_oldbuttonstate;
int				mouse_x;
int				mouse_y;
int				mx_accum;
int				my_accum;

unsigned int	uiWheelMessage;

qboolean		mouseactive;
qboolean		mouseinitialized;
static qboolean	mouseactivatetoggle;

/*
==================
Force_CenterView_f
==================
*/
void Force_CenterView_f (void)
{
	cl.viewangles[PITCH] = 0;
}

/*
===================
IN_UpdateClipCursor
===================
*/
void IN_UpdateClipCursor (void)
{
	if (mouseinitialized && mouseactive)	ClipCursor (&window_rect);
}

/*
================
IN_ActivateMouse
================
*/
void IN_ActivateMouse (void)
{
	mouseactivatetoggle = true;

	if (mouseinitialized)
	{
		SetCursorPos (window_center_x, window_center_y);
		SetCapture (mainwindow);
		ClipCursor (&window_rect);

		mouseactive = true;
	}
}

/*
=====================
IN_SetQuakeMouseState
=====================
*/
void IN_SetQuakeMouseState (void)
{
	if (mouseactivatetoggle)
		IN_ActivateMouse ();
}

/*
==================
IN_DeactivateMouse
==================
*/
void IN_DeactivateMouse (void)
{
	mouseactivatetoggle = false;

	if (mouseinitialized)
	{
		ClipCursor (NULL);
		ReleaseCapture ();

		mouseactive = false;
	}
}

/*
============================
IN_RestoreOriginalMouseState
============================
*/
void IN_RestoreOriginalMouseState (void)
{
	if (mouseactivatetoggle)
	{
		IN_DeactivateMouse ();
		mouseactivatetoggle = true;
	}

	ShowCursor (TRUE);
	ShowCursor (FALSE);
}

/*
===============
IN_StartupMouse
===============
*/
void IN_StartupMouse (void)
{
	if ( COM_CheckParm ("-nomouse") ) 
		return; 

	mouseinitialized = true;

	mouse_buttons = 3;

	if (mouseactivatetoggle)
		IN_ActivateMouse ();
}

/*
=======
IN_Init
=======
*/
void IN_Init (void)
{
	Cmd_AddCommand ("force_centerview", Force_CenterView_f);

	uiWheelMessage = RegisterWindowMessage ("MSWHEEL_ROLLMSG");

	IN_StartupMouse ();
}

/*
===========
IN_Shutdown
===========
*/
void IN_Shutdown (void)
{

	IN_DeactivateMouse ();
	ShowCursor (TRUE);
}

/*
=============
IN_MouseEvent
=============
*/
void IN_MouseEvent (int mstate)
{
	int	i;

	if (mouseactive)
	{
	// perform button actions
		for (i=0 ; i<mouse_buttons ; i++)
		{
			if ( (mstate & (1<<i)) &&
				!(mouse_oldbuttonstate & (1<<i)) )
			{
				Key_Event (K_MOUSE1 + i, true);
			}

			if ( !(mstate & (1<<i)) &&
				(mouse_oldbuttonstate & (1<<i)) )
			{
				Key_Event (K_MOUSE1 + i, false);
			}
		}	
			
		mouse_oldbuttonstate = mstate;
	}
}

/*
============
IN_MouseMove
============
*/
void IN_MouseMove (usercmd_t *cmd)
{
	int	mx, my;

	if (!mouseactive)
		return;

	GetCursorPos (&current_pos);
	mx = current_pos.x - window_center_x + mx_accum;
	my = current_pos.y - window_center_y + my_accum;
	mx_accum = 0;
	my_accum = 0;

	mouse_x = mx;
	mouse_y = my;

	mouse_x *= sensitivity.value;
	mouse_y *= sensitivity.value;

// add mouse X/Y movement to cmd
	if ( (in_strafe.state & 1) || (lookstrafe.value && (in_mlook.state & 1) ))
		cmd->sidemove += m_side.value * mouse_x;
	else
		cl.viewangles[YAW] -= m_yaw.value * mouse_x;

	if (in_mlook.state & 1)
		V_StopPitchDrift ();
		
	if ( (in_mlook.state & 1) && !(in_strafe.state & 1))
	{
		cl.viewangles[PITCH] += m_pitch.value * mouse_y;
		if (cl.viewangles[PITCH] > 80)
			cl.viewangles[PITCH] = 80;
		if (cl.viewangles[PITCH] < -70)
			cl.viewangles[PITCH] = -70;
	}
	else
	{
		if ((in_strafe.state & 1) && noclip_anglehack)
			cmd->upmove -= m_forward.value * mouse_y;
		else
			cmd->forwardmove -= m_forward.value * mouse_y;
	}

// if the mouse has moved, force it to the center, so there's room to move
	if (mx || my)
	{
		SetCursorPos (window_center_x, window_center_y);
	}
}

/*
=======
IN_Move
=======
*/
void IN_Move (usercmd_t *cmd)
{
	if (ActiveApp && !Minimized)	IN_MouseMove (cmd);
}

/*
=============
IN_Accumulate
=============
*/
void IN_Accumulate (void)
{
	if (mouseactive)
	{
		GetCursorPos (&current_pos);

		mx_accum += current_pos.x - window_center_x;
		my_accum += current_pos.y - window_center_y;

		SetCursorPos (window_center_x, window_center_y);
	}
}


/*
==============
IN_ClearStates
==============
*/
void IN_ClearStates (void)
{

	if (mouseactive)
	{
		mx_accum = 0;
		my_accum = 0;
		mouse_oldbuttonstate = 0;
	}
}
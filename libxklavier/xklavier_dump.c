/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <string.h>

#include "xklavier_private.h"

static void _XkbModsRecDump( FILE * fs, XkbModsRec * mods )
{
  fprintf( fs, "flags: 0x%X\n", mods->mask );
  fprintf( fs, "real_mods: 0x%X\n", mods->real_mods );
  fprintf( fs, "vmods: 0x%X\n", mods->vmods );
}

static void _XkbControlsDump( FILE * fs, XkbControlsPtr ctrls )
{
  int i;
  char buf[1024];
  fprintf( fs, "mk_dflt_btn: %d\n", ctrls->mk_dflt_btn );
  fprintf( fs, "num_groups: %d\n", ctrls->num_groups );
  fprintf( fs, "groups_wrap: %d\n", ctrls->groups_wrap );
  fprintf( fs, "internal: \n" );
  _XkbModsRecDump( fs, &ctrls->internal );
  fprintf( fs, "ignore_lock: \n" );
  _XkbModsRecDump( fs, &ctrls->ignore_lock );
  fprintf( fs, "enabled_ctrls: 0x%X\n", ctrls->enabled_ctrls );
  fprintf( fs, "repeat_delay: %d\n", ctrls->repeat_delay );
  fprintf( fs, "repeat_interval: %d\n", ctrls->repeat_interval );
  fprintf( fs, "slow_keys_delay: %d\n", ctrls->slow_keys_delay );
  fprintf( fs, "debounce_delay: %d\n", ctrls->debounce_delay );
  fprintf( fs, "mk_delay: %d\n", ctrls->mk_delay );
  fprintf( fs, "mk_interval: %d\n", ctrls->mk_interval );
  fprintf( fs, "mk_time_to_max: %d\n", ctrls->mk_time_to_max );
  fprintf( fs, "mk_max_speed: %d\n", ctrls->mk_max_speed );
  fprintf( fs, "mk_curve: %d\n", ctrls->mk_curve );
  fprintf( fs, "ax_options: %d\n", ctrls->ax_options );
  fprintf( fs, "ax_timeout: %d\n", ctrls->ax_timeout );
  fprintf( fs, "axt_opts_mask: 0x%X\n", ctrls->axt_opts_mask );
  fprintf( fs, "axt_opts_values: 0x%X\n", ctrls->axt_opts_values );
  fprintf( fs, "axt_ctrls_mask: 0x%X\n", ctrls->axt_ctrls_mask );
  fprintf( fs, "axt_ctrls_values: 0x%X\n", ctrls->axt_ctrls_values );
  fprintf( fs, "axt_ctrls_values: 0x%X\n", ctrls->axt_ctrls_values );
  fprintf( fs, "per_key_repeat:\n" );
  buf[0] = 0;
  for( i = 0; i < XkbPerKeyBitArraySize; i++ )
  {
    char b[5];
    snprintf( b, sizeof( b ), "%d ", ctrls->per_key_repeat[i] );
    strcat( buf, b );
  }
  fprintf( fs, "  %s\n", buf );
}

static const char *actionTypeNames[] = {
  "XkbSA_NoAction",
  "XkbSA_SetMods",
  "XkbSA_LatchMods",
  "XkbSA_LockMods",
  "XkbSA_SetGroup",
  "XkbSA_LatchGroup",
  "XkbSA_LockGroup",
  "XkbSA_MovePtr",
  "XkbSA_PtrBtn",
  "XkbSA_LockPtrBtn",
  "XkbSA_SetPtrDflt",
  "XkbSA_ISOLock",
  "XkbSA_Terminate",
  "XkbSA_SwitchScreen",
  "XkbSA_SetControls",
  "XkbSA_LockControls",
  "XkbSA_ActionMessage",
  "XkbSA_RedirectKey",
  "XkbSA_DeviceBtn",
  "XkbSA_LockDeviceBtn",
  "XkbSA_DeviceValuator"
};

static void _XkbActionDump( FILE * fs, int level, XkbAction * act )
{
  XkbGroupAction *ga;
  fprintf( fs, "%*stype: %d(%s)\n", level, "", act->type,
           actionTypeNames[act->type] );
  switch ( act->type )
  {
    case XkbSA_SetGroup:
    case XkbSA_LatchGroup:
    case XkbSA_LockGroup:
      ga = ( XkbGroupAction * ) act;
      fprintf( fs, "%*sXkbGroupAction: \n", level, "" );
      fprintf( fs, "%*sflags: %d\n", level, "", ga->flags );
      fprintf( fs, "%*sgroup_XXX: %d\n", level, "", ga->group_XXX );
      break;
  }
}

static void _XkbBehaviorDump( FILE * fs, int level, XkbBehavior * b )
{
  fprintf( fs, "%*stype: %d\n", level, "", b->type );
  fprintf( fs, "%*sdata: %d\n", level, "", b->data );
}

static void _XkbServerMapDump( FILE * fs, int level, XkbServerMapPtr server,
                               XkbDescPtr kbd )
{
  int i;
  XkbAction *pa = server->acts;
  XkbBehavior *pb = server->behaviors;
  fprintf( fs, "%*snum_acts: %d\n", level, "", server->num_acts );
  fprintf( fs, "%*ssize_acts: %d\n", level, "", server->size_acts );
  if( server->acts != NULL )
  {
    for( i = 0; i < server->num_acts; i++ )
    {
      fprintf( fs, "%*sacts[%d]:\n", level, "", i );
      _XkbActionDump( fs, level + 2, pa++ );
    }
  } else
    fprintf( fs, "%*sNO acts\n", level, "" );

  if( server->key_acts != NULL )
  {
    for( i = 0; i <= kbd->max_key_code; i++ )
    {
      fprintf( fs, "%*skey_acts[%d]: offset %d, total %d\n", level, "", i,
               server->key_acts[i], XkbKeyNumSyms(kbd,i) );
    }
  } else
    fprintf( fs, "%*sNO key_acts\n", level, "" );

  for( i = 0; i < XkbNumVirtualMods; i++ )
  {
    fprintf( fs, "%*svmod[%d]: %X\n", level, "", i, server->vmods[i] );
  }

  if( server->behaviors != NULL )
  {
    for( i = 0; i <= kbd->max_key_code; i++ )
    {
      fprintf( fs, "%*sbehaviors[%d]:\n", level, "", i );
      _XkbBehaviorDump( fs, level + 2, pb++ );
    }
  } else
    fprintf( fs, "%*sNO behaviors\n", level, "" );

  if( server->explicit != NULL )
  {
    for( i = 0; i <= kbd->max_key_code; i++ )
    {
      fprintf( fs, "%*sexplicit[%d]: %d\n", level, "", i,
               server->explicit[i] );
    }
  } else
    fprintf( fs, "%*sNO explicit\n", level, "" );

  if( server->vmodmap != NULL )
  {
    for( i = 0; i <= kbd->max_key_code; i++ )
    {
      fprintf( fs, "%*svmodmap[%d]: %d\n", level, "", i, server->vmodmap[i] );
    }
  } else
    fprintf( fs, "%*sNO vmodmap\n", level, "" );
}

static void _XkbKeyTypeDump( FILE * fs, int level, XkbKeyTypePtr type )
{
  char *z = type->name == None ? NULL : XGetAtomName( _xklDpy, type->name );
  fprintf( fs, "%*sname: 0x%X(%s)\n", level, "", type->name, z );
  if( z != NULL )
    XFree( z );
}

static void _XkbSymMapDump( FILE * fs, int level, XkbSymMapPtr ksm )
{
  int i;
  fprintf( fs, "%*skt_index: ", level, "" );
  for( i = 0; i < XkbNumKbdGroups; i++ )
  {
    fprintf( fs, "%d ", ksm->kt_index[i] );
  }
  fprintf( fs, "\n%*sgroup_info: %d\n", level, "", ksm->group_info );
  fprintf( fs, "%*swidth: %d\n", level, "", ksm->width );
  fprintf( fs, "%*soffset: %d\n", level, "", ksm->offset );
}

static void _XkbClientMapDump( FILE * fs, int level, XkbClientMapPtr map,
                               XkbDescPtr kbd )
{
  int i;
  fprintf( fs, "%*ssize_types: %d\n", level, "", map->size_types );
  fprintf( fs, "%*snum_types: %d\n", level, "", map->num_types );
  if( map->types != NULL )
  {
    XkbKeyTypePtr type = map->types;
    for( i = 0; i < map->num_types; i++ )
    {
      fprintf( fs, "%*stypes[%d]:\n", level, "", i );
      _XkbKeyTypeDump( fs, level + 2, type++ );
    }
  } else
    fprintf( fs, "%*sNO types\n", level, "" );

  fprintf( fs, "%*ssize_syms: %d\n", level, "", map->size_syms );
  fprintf( fs, "%*snum_syms: %d\n", level, "", map->num_syms );
  if( map->syms != NULL )
  {
    for( i = 0; i < map->num_syms; i++ )
      fprintf( fs, "%*ssyms[%d]:0x%lX(%s)\n", level, "", i, map->syms[i],
               XKeysymToString( map->syms[i] ) );
  } else
    fprintf( fs, "%*sNO syms\n", level, "" );
  if( map->key_sym_map != NULL )
  {
    XkbSymMapPtr ksm = map->key_sym_map;
    for( i = 0; i <= kbd->max_key_code; i++ )
    {
      fprintf( fs, "%*skey_sym_map[%d]:\n", level, "", i );
      _XkbSymMapDump( fs, level + 2, ksm++ );
    }
  } else
    fprintf( fs, "%*sNO key_sym_map\n", level, "" );
}

void _XkbDescDump( FILE * fs, int level, XkbDescPtr kbd )
{
  fprintf( fs, "%*sflags: 0x%X\n", level, "", kbd->flags );
  fprintf( fs, "%*sdevice_spec: %d\n", level, "", kbd->device_spec );
  fprintf( fs, "%*smin_key_code: %d\n", level, "", kbd->min_key_code );
  fprintf( fs, "%*smax_key_code: %d\n", level, "", kbd->max_key_code );
#if 0
  if( kbd->ctrls != NULL )
  {
    fprintf( fs, "ctrls:\n" );
    _XkbControlsDump( fs, kbd->ctrls );
  } else
    fprintf( fs, "NO server\n" );
#endif
  if( kbd->server != NULL )
  {
    fprintf( fs, "%*sserver:\n", level, "" );
    _XkbServerMapDump( fs, level + 2, kbd->server, kbd );
  } else
    fprintf( fs, "%*sNO server\n", level, "" );

  if( kbd->map != NULL )
  {
    fprintf( fs, "%*smap:\n", level, "" );
    _XkbClientMapDump( fs, level + 2, kbd->map, kbd );
  } else
    fprintf( fs, "%*sNO map\n", level, "" );
}

void XklDumpXkbDesc( const char *filename, XkbDescPtr kbd )
{
  FILE *fs = fopen( filename, "w+" );
  if( fs != NULL )
  {
    _XkbDescDump( fs, 0, kbd == NULL ? _xklXkb : kbd );
    fclose( fs );
  }

}

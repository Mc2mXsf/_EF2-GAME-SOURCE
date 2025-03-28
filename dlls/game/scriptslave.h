//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/scriptslave.h                                 $
// $Revision:: 29                                                             $
//   $Author:: Steven                                                         $
//     $Date:: 10/13/03 8:54a                                                 $
//
// Copyright (C) 1997 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Standard scripted objects.  Controlled by scriptmaster.  These objects
// are bmodel objects created in the editor and controlled by an external
// text based script.  Commands are interpretted on by one and executed
// upon a signal from the script master.  The base script object can
// perform several different relative and specific rotations and translations
// and can cause other parts of the script to be executed when touched, damaged,
// touched, or used.
//

#ifndef __SCRIPTSLAVE_H__
#define __SCRIPTSLAVE_H__

#include "g_local.h"
#include "entity.h"
#include "trigger.h"
#include "mover.h"
#include "script.h"
#include "scriptmaster.h"
#include "misc.h"
#include "bspline.h"

extern Event EV_ScriptSlave_NewOrders;

class ScriptSlave : public Trigger
	{
	private:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		Vector				_newAngles = Vector(0.0f, 0.0f, 0.0f);
		Vector				_newPosition = Vector(0.0f, 0.0f, 0.0f);
		bool				_handlesDamage = false;

	protected:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		ThreadPtr			touchthread = nullptr;
		ThreadPtr			blockthread = nullptr;
		ThreadPtr			triggerthread = nullptr;
		ThreadPtr			usethread = nullptr;
		ThreadPtr			damagethread = nullptr;
		ThreadPtr			movethread = nullptr;


		str					touchlabel;
		str					uselabel;
		str					blocklabel;
		str					triggerlabel;
		str					damagelabel;


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		float				attack_finished = 0.0f;
		float				dmg = 0.0f;
		int					dmg_means_of_death = 0;
		qboolean			thinking = qfalse;
		float				_forcefieldNumber = 0.0f;
		float				_triggerNumber = 0.0f;
		EntityPtr			_scanner = nullptr;
		bool				_portalOpen = false;
		float				_nextNeedToUseTime = 0.0f;
		bool				_canBeAttackedByOtherScriptSlaves = false;


		str					_damageEffect;
		str					_bloodModel;
		Container<str>		_requiredMODlist;


	public:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean			commandswaiting = qfalse;
		Vector				TotalRotation = Vector(0.0f, 0.0f, 0.0f);
		Vector				ForwardDir = Vector(0.0f, 0.0f, 0.0f);
		float				speed = 0.0f;
		Waypoint			*waypoint = nullptr;
		float				traveltime = 0.0f;
		BSpline				*splinePath = nullptr;
		float				splineTime = 0.0f;
		SplinePathPtr		currentNode = nullptr;
		int					lastTime = 0;
		int					newTime = 0;
		qboolean			splineangles = qfalse;
		qboolean			ignoreangles = qfalse;
		qboolean			moving = qfalse;  // is the script object currently moving?


		CLASS_PROTOTYPE( ScriptSlave );

							ScriptSlave();
							~ScriptSlave();

		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Weapon accuracy bug, not counting destructibles. - chrissstrahl
		//--------------------------------------------------------------
		str					GetDamageLabel( void );

		
		void				StopEvent( Event *ev );
		void				Stop( void );
		void				ContinueEvent( Event *ev );
		void				Continue( void );
		void				Think(void);

		const Vector &		GetNewAngles( void ) const { return _newAngles; }
		void				SetNewAngles( const Vector &newAngles ){ _newAngles = newAngles; }
		const Vector &		GetNewPosition( void ) const { return _newPosition; }
		void				SetNewPosition( const Vector &newPosition ) { _newPosition = newPosition; }
		virtual void		setOrigin( const Vector &point );
		virtual void		setOrigin( void );
		void				NewOrders( Event *ev );
		void				BindEvent( Event *ev );
		void				EventUnbind( Event *ev );
		void				DoMove( Event *ev );
		void				MoveEnd( Event *ev );
		void				SetAnglesEvent( Event *ev );
		void				SetAngleEvent( Event *ev );
		void				SetModelEvent( Event *ev );
		void				TriggerEvent( Event *ev );
		void				GotoNextWaypoint( Event *ev );
		void				JumpTo( Event *ev );
		void				MoveToEvent( Event *ev );
		void				MoveToPositionEvent( Event *ev );
		void				SetSpeed( Event *ev );
		void				SetTime( Event *ev );
		void				MoveUp( Event *ev );
		void				MoveDown( Event *ev );
		void				MoveNorth( Event *ev );
		void				MoveSouth( Event *ev );
		void				MoveEast( Event *ev );
		void				MoveWest( Event *ev );
		void				MoveForward( Event *ev );
		void				MoveBackward( Event *ev );
		void				MoveLeft( Event *ev );
		void				MoveRight( Event *ev );
		void				RotateXdownto( Event *ev );
		void				RotateYdownto( Event *ev );
		void				RotateZdownto( Event *ev );
		void				RotateAxisdownto( Event *ev );
		void				RotateXupto( Event *ev );
		void				RotateYupto( Event *ev );
		void				RotateZupto( Event *ev );
		void				RotateAxisupto( Event *ev );
		void				Rotateupto( Event *ev );
		void				Rotatedownto( Event *ev );
		void				Rotateto( Event *ev );
		void				RotateXdown( Event *ev );
		void				RotateYdown( Event *ev );
		void				RotateZdown( Event *ev );
		void				RotateAxisdown( Event *ev );
		void				RotateXup( Event *ev );
		void				RotateYup( Event *ev );
		void				RotateZup( Event *ev );
		void				RotateAxisup( Event *ev );
		void				RotateX( Event *ev );
		void				RotateY( Event *ev );
		void				RotateZ( Event *ev );
		void				RotateAxis( Event *ev );
		void				OnTouch( Event *ev );
		void				NoTouch( Event *ev );
		void				TouchFunc( Event *ev );
		void				OnBlock( Event *ev );
		void				NoBlock( Event *ev );
		void				BlockFunc( Event *ev );
		void				OnTrigger( Event *ev );
		void				NoTrigger( Event *ev );
		void				TriggerFunc( Event *ev );
		void				OnUse( Event *ev );		
		void				NoUse( Event *ev );
		void				UseFunc( Event *ev );
		void				OnDamage( Event *ev );
		void				NoDamage( Event *ev );
		void				DamageFunc( Event *ev );
		void				SetDamage( Event *ev );
		void				SetMeansOfDeath( Event *ev );
		void				FollowPath( Event *ev );
		void				EndPath( Event *ev );
		void				FollowingPath( Event *ev );
		void				CreatePath( SplinePath *path,	splinetype_t type );
		void				Explode( Event *ev );
		void				NotShootable( Event *ev );
		void				OpenPortal( Event *ev );
		void				ClosePortal( Event *ev );
		void				PhysicsOn( Event *ev );
		void				PhysicsOff( Event *ev );
		void				PhysicsVelocity( Event *ev );
		void				setDamage( Event *ev );
		void				HandlesDamage( Event *ev );		
		void				setDamageEffect( Event *ev );
		void				setBloodModel( Event *ev );
		void				addRequiredDamageMOD( Event *ev );
		void				setCanBeAttackedByOtherSlaves( Event *ev );


		// 1ST PLAYABLE HACK
		void				Hack_AddParms( Event *ev );
		void				Hack_GetForceFieldNumber( Event *ev );
		void				Hack_GetTriggerNumber( Event *ev );
		void				Hack_GetScanner( Event *ev );

		virtual void		Archive( Archiver &arc );
	};

inline void ScriptSlave::Archive( Archiver &arc )
{
	int tempInt;

	Trigger::Archive( arc );

	arc.ArchiveVector( &_newAngles );
	arc.ArchiveVector( &_newPosition );
	arc.ArchiveBool( &_handlesDamage );

	arc.ArchiveSafePointer( &touchthread );
	arc.ArchiveSafePointer( &blockthread );
	arc.ArchiveSafePointer( &triggerthread );
	arc.ArchiveSafePointer( &usethread );
	arc.ArchiveSafePointer( &damagethread );
	arc.ArchiveSafePointer( &movethread );

	arc.ArchiveString( &touchlabel );
	arc.ArchiveString( &uselabel );
	arc.ArchiveString( &blocklabel );
	arc.ArchiveString( &triggerlabel );
	arc.ArchiveString( &damagelabel );

	arc.ArchiveFloat( &attack_finished );
	arc.ArchiveFloat( &dmg );
	arc.ArchiveInteger( &dmg_means_of_death );
	arc.ArchiveBoolean( &thinking );

	arc.ArchiveFloat( &_forcefieldNumber );
	arc.ArchiveFloat( &_triggerNumber );
	arc.ArchiveSafePointer( &_scanner );

	arc.ArchiveBool( &_portalOpen );

	arc.ArchiveString( &_damageEffect );
	arc.ArchiveString( &_bloodModel );
	

	if( arc.Saving() )
	{
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C4458: declaration of count hides class member. Renamed to: temp_count - chrissstrahl
		//--------------------------------------------------------------
		int temp_count = _requiredMODlist.NumObjects();


		arc.ArchiveInteger( &temp_count );
		for( int i = 1; i <= _requiredMODlist.NumObjects(); i++ )
			arc.ArchiveString( &_requiredMODlist.ObjectAt( i ) );
	}
	else
	{
		assert( _requiredMODlist.NumObjects() == 0 );


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C4458: declaration of count hides class member. Renamed to: temp_count - chrissstrahl
		//--------------------------------------------------------------
		int temp_count;


		arc.ArchiveInteger( &temp_count );

		str s;
		for( int i = 0; i < temp_count; i++ )
		{
			arc.ArchiveString( &s );
			_requiredMODlist.AddObject( s );
		}
	}


	arc.ArchiveFloat( &_nextNeedToUseTime );

	arc.ArchiveBoolean( &commandswaiting );
	arc.ArchiveVector( &TotalRotation );

	arc.ArchiveVector( &ForwardDir );
	arc.ArchiveFloat( &speed );
	arc.ArchiveObjectPointer( ( Class ** )&waypoint );
	arc.ArchiveFloat( &traveltime );

	if ( arc.Saving() )
		{
		// if it exists, archive it, otherwise place a special NULL ptr tag
		if ( splinePath )
			{
			tempInt = ARCHIVE_POINTER_VALID;
			}
		else
			{
			tempInt = ARCHIVE_POINTER_NULL;
			}
		arc.ArchiveInteger( &tempInt );
		if ( tempInt == ARCHIVE_POINTER_VALID )
			{
			splinePath->Archive( arc );
			}
	  }
	else
		{
		arc.ArchiveInteger( &tempInt );
		if ( tempInt == ARCHIVE_POINTER_VALID )
			{
			splinePath = new BSpline;
			splinePath->Archive( arc );
			}
	  else
			{
			splinePath = NULL;
			}
		}
	arc.ArchiveFloat( &splineTime );

	arc.ArchiveSafePointer( &currentNode );
	arc.ArchiveInteger( &lastTime );
	arc.ArchiveInteger( &newTime );

	arc.ArchiveBoolean( &splineangles );
	arc.ArchiveBoolean( &ignoreangles );
	arc.ArchiveBoolean( &moving );
	arc.ArchiveBool( &_canBeAttackedByOtherScriptSlaves );
   
}


class ScriptModel : public ScriptSlave
{
	private:
		void				GibEvent(Event *ev);


		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean			animationDriven = qfalse;


	public:
		CLASS_PROTOTYPE( ScriptModel );

								ScriptModel();
		virtual void			Think( void );
		void					SetAngleEvent( Event *ev );
		void					SetModelEvent( Event *ev );
		void					SetAnimEvent( Event *ev );
		void					SetAnimDrivenEvent( Event *ev );
		void					AnimOnceEvent( Event *ev );		
		virtual void			Archive( Archiver &arc );
};

inline void ScriptModel::Archive(Archiver& arc)
{
	ScriptSlave::Archive( arc );

	arc.ArchiveBoolean( &animationDriven );
}


class ScriptOrigin : public ScriptSlave
{
	public:
		CLASS_PROTOTYPE( ScriptOrigin );
						ScriptOrigin();
};

class ScriptSkyOrigin : public ScriptSlave
	{
	public:
		//--------------------------------------------------------------
		// GAMEFIX - Fixed: Warning C26495: The Variable ? was not initialized. A Membervariable needs always to be initialized (type.6) - chrissstrahl
		//--------------------------------------------------------------
		qboolean			use_base_position = qfalse;
		Vector				base_position = Vector(0.0f, 0.0f, 0.0f);
		qboolean			use_player_base_position = qfalse;
		Vector				player_base_position = Vector(0.0f, 0.0f, 0.0f);
		float				translation_multiplier = 0.0f;
		float				max_distance = 0.0f;


		CLASS_PROTOTYPE( ScriptSkyOrigin );

							ScriptSkyOrigin();
		void				Think( void );
		void				SetBasePosition( Event *ev );
		void				SetPlayerBasePosition( Event *ev );
		void				SetTranslationMultiplier( Event *ev );
		void				SetMaxDistance( Event *ev );
		virtual void		Archive( Archiver &arc );
	};

inline void ScriptSkyOrigin::Archive( Archiver &arc )
{
	ScriptSlave::Archive( arc );

	arc.ArchiveBoolean( &use_base_position );
	arc.ArchiveVector( &base_position );
	arc.ArchiveBoolean( &use_player_base_position );
	arc.ArchiveVector( &player_base_position );
	arc.ArchiveFloat( &translation_multiplier );
	arc.ArchiveFloat( &max_distance );
}

#endif /* scriptslave.h */

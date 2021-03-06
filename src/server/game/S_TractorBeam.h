/*
 * S_TractorBeam.h
 */

#ifndef S_TRACTORBEAM_H_INCLUDED
#define S_TRACTORBEAM_H_INCLUDED

// External includes
#define WIN32_LEAN_AND_MEAN
#include <d3dx9.h>

// Project includes
#include <shared/game/Entity.h>
#include <shared/game/TractorBeam.h>
#include <server/game/ServerEntity.h>
#include <server/game/S_Ship.h>
#include <server/Globals.h>

class S_Ship;

#pragma warning( push )
#pragma warning( disable : 4250 )

class S_TractorBeam : public TractorBeam, public ServerEntity{ 
private:
	bool m_isPulling;
public:
	// Fields
	float m_power;
	
	long disableLength;
	long disableStart;

	ServerEntity * m_lastHeld;
	long m_lastHeldTimer;

	ServerEntity * m_object;
	S_Ship * m_ship; 
	float m_strength;
	
	bool m_isColliding;
	bool m_isHolding;
	float m_heldDistance; //temp


	// Constructors
	S_TractorBeam(S_Ship *);

	// Methods
	bool isLocked();
	float getCurrentDistance();
	D3DXVECTOR3 getCurrentDirection();
	D3DXVECTOR3 getCurrentDistanceVector();
	D3DXVECTOR3 getDistanceVectorOf(D3DXVECTOR3);
	void setStartPoint();
	void setEndPoint();
	void lockOn(ServerEntity * entity);
	void lockOff();
	void timeout(long);
	void disable(long);

	void setIsPulling(bool);
	bool getIsPulling();
	void setIsOn(bool);

	bool interact(ServerEntity *);
	
	// For applying impulses, updating data, etc
	void updateData();

	void calculateForce();
	
	// Overwrite, since doesnt do anything during normal cycle
	void update(float delta_time);

	virtual D3DXMATRIX calculateRotationalInertia(float mass);
};

#pragma warning( pop )

#endif S_TRACTORBEAM_H_INCLUDED
/*
 * Entity.h
 */

#ifndef ENTITIY_H_INCLUDED
#define ENTITIY_H_INCLUDED
#pragma once

// Defines
#define WIN32_LEAN_AND_MEAN

// Global includes
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
#include <memory>

// Project includes
#include <shared/network/Sendable.h>
#include <client/graphics/Renderable.h>

using namespace std;

typedef D3DXQUATERNION Quaternion;

#define ENUM_TYPE char
enum Type : char { ENTITY, SHIP, BASE, ASTEROID};

class Entity : public Sendable {
private:
	static int s_id_gen;
	
protected:
	static const unsigned int m_size = sizeof(ENUM_TYPE) + sizeof(int) +  2*sizeof(D3DXVECTOR3);
	
	// THIS CONSTRUCTOR SHOULD NEVER BE DIRECTLY CALLED.
	Entity();

public:
	// ID AND TYPE SHOULD BOTH BE CONST
	//const int m_id;
	//const Type m_type;
	int m_id;
	Type m_type;

	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_dir; // DEPRECATED
	Quaternion m_orientation;

	// These two constructors are DEPRECATED
	Entity(Type type);
	Entity(Type type, D3DXVECTOR3 pos, D3DXVECTOR3 dir);

	// Constructors
	Entity(int id, Type type);
	Entity(int id, Type type, D3DXVECTOR3 pos, Quaternion orientation);
	// Copy constructor
	Entity(Entity const &e);
	// Destructor
	virtual ~Entity();

	// Methods
	const int getID() const { return m_id; };
	virtual const char* encode() const;
	virtual void decode(const char *);
	virtual const unsigned int size() const { return m_size; };
	virtual void update(shared_ptr<Entity> source);
	//virtual void update(Entity* source);

	friend ostream& operator<<(ostream& os, const Entity& e);	
};


#endif
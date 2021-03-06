/*
 * Asteroid.cpp
 */

// Project includes
#include <shared/game/Asteroid.h>

Asteroid::Asteroid() :
	Entity(ASTEROID),
	m_scale(1.0f)
{
}

Asteroid::Asteroid(float scale) :
	Entity(ASTEROID),
	m_scale(scale)
{
}


unsigned int Asteroid::encode(char *head) const {
	// Get entity encode
	unsigned int rtn = Entity::encode(head);

	// Encode Scale
	*(float *) (head+rtn) = m_scale;
	rtn += sizeof(float);

	return rtn;
}

ostream& operator<<(ostream& os, const Asteroid& e) {
	os << e.getID() << " " << e.m_pos.x << " " << e.m_pos.y << " " << e.m_pos.z
		<< " " << e.m_orientation.x << " " << e.m_orientation.y << " " << e.m_orientation.z << " " << e.m_orientation.w
		<< " " << e.m_scale;
	return os;
}

unsigned int Asteroid::decode(const char *buff) {
	unsigned int rtn =Entity::decode(buff);
	m_scale = *(float*) (buff+rtn);
	rtn += sizeof(float);
	return rtn;
}


void Asteroid::update(shared_ptr<Entity> sp_source) {
	Entity *source = sp_source.get();
	Asteroid * srcAsteroid = dynamic_cast<Asteroid*>(source);
	if (srcAsteroid == 0) {
		//TODO figure out how to handle this case when not in debug
#ifdef _DEBUG
//TODO put this back in, make sure it doesn't break anyone's build
//		MessageBox( NULL, L"Error converting Entity to Ship", L"CRUSH Game", MB_OK );
#endif
	} else {
		Entity::update(sp_source);
		m_scale = srcAsteroid->m_scale;
	}
}
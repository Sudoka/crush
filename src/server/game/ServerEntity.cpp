#include <shared/game/Entity.h>
#include <server/game/ServerEntity.h>

float FP_ZERO = 0.000001f;
D3DXVECTOR3 zero_vec(0.0f, 0.0f, 0.0f);

int ServerEntity::s_id_gen = 0;

ServerEntity::ServerEntity() :
	m_velocity(0.0f, 0.0f, 0.0f),
	m_max_velocity(1),
	m_mass(1),
	m_mass_inverse(1),
	m_radius(1),
	m_immovable(false)
{ 
	reset();
}

ServerEntity::ServerEntity(D3DXVECTOR3 velocity, float max_velocity, float mass) :
	m_velocity(velocity),
	m_max_velocity(max_velocity),
	m_mass(mass),
	m_mass_inverse(1/mass),
	m_radius(1),
	m_immovable(false)
{ 
	reset();
}


ServerEntity::ServerEntity(float max_velocity, float max_angular_velocity, float mass, D3DXVECTOR3 rot_inertia) :
	m_max_velocity(max_velocity),
	m_max_angular_velocity(max_angular_velocity),
	m_mass(mass),
	m_mass_inverse(1/mass),
	m_rot_inertia(rot_inertia),
	m_rot_inertia_inverse(1/rot_inertia.x, 1/rot_inertia.y, 1/rot_inertia.z),
	m_radius(1),
	// zeroing values
	m_velocity(zero_vec),
	m_angular_velocity(zero_vec),
	m_orientation_delta(0.0f, 0.0f, 0.0f, 0.0f),
	m_momentum(zero_vec),
	m_angular_momentum(zero_vec),
	t_impulse(zero_vec),
	t_angular_impulse(zero_vec),
	m_immovable(false)
{ }

int ServerEntity::genId() {
	return s_id_gen++;
}

// Physics Methods
// Applies a force to this entity's center of mass for the given amount of time(will not cause rotation)
void ServerEntity::applyImpulse(D3DXVECTOR3 force, float duration) {
	// Linear calculation
	t_impulse += force * duration;
}

// Applies a force to this entity at the given point for the given amount of time
void ServerEntity::applyImpulse(D3DXVECTOR3 force, D3DXVECTOR3 point, float duration) {
	// Linear calculation
	t_impulse += force * duration;

	// Rotational calcultation
	D3DXVECTOR3 torque, vector_to_point = point - m_pos;
	D3DXVec3Cross(&torque, &force, &vector_to_point); // Cross product finds torque
	t_angular_impulse += torque * duration;
}

// Applies current acceleration to Entity
void ServerEntity::update(float delta_time) {
	// Move according to last known values for half time
	float half_time = (delta_time / 2);
	m_pos += m_velocity * half_time;
	m_orientation += m_orientation_delta * half_time;
	D3DXQuaternionNormalize(&m_orientation, &m_orientation);

	// Apply impulse
	m_momentum += t_impulse;
	m_angular_momentum += t_angular_impulse;
	
	// Reset temporary values
	t_impulse = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	t_angular_impulse = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// Calculate velocity/angular velocity
	m_velocity = m_momentum * m_mass_inverse;
	m_angular_velocity = D3DXVECTOR3(m_angular_momentum.x * m_rot_inertia_inverse.x, 
									 m_angular_momentum.y * m_rot_inertia_inverse.y,
									 m_angular_momentum.z * m_rot_inertia_inverse.z);
	if (D3DXVec3LengthSq(&m_angular_velocity) > FP_ZERO) {
		m_orientation_delta = 0.5 *(m_orientation * Quaternion(m_angular_velocity.x, m_angular_velocity.y, m_angular_velocity.z, 0.0f));
	} else {
		D3DXQuaternionIdentity(&m_orientation_delta);
	}

	m_pos += m_velocity * half_time;
	m_orientation += (m_orientation_delta * half_time);
	D3DXQuaternionNormalize(&m_orientation, &m_orientation);
	D3DXQuaternionNormalize(&m_orientation, &m_orientation);
}

// Getters/Setters
void ServerEntity::setVelocity(D3DXVECTOR3 velocity) {
	m_velocity = velocity;
}

D3DXVECTOR3 ServerEntity::getVelocity() {
	return m_velocity;
}


void ServerEntity::rotate(float angle, D3DXVECTOR3 axis){
	D3DXQuaternionRotationAxis(&m_orientation, &axis, angle);
}

void ServerEntity::move(D3DXVECTOR3 vector){
	m_pos += vector;
}

void ServerEntity::reset() {
	cout << "Resetting!" << endl;

	// Derived Variables
	m_velocity = zero_vec;
	m_angular_velocity = zero_vec;
	m_orientation_delta = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);

	// Calculated Values
	m_momentum = zero_vec;
	m_angular_momentum = zero_vec;

	// Accumulator Values
	t_impulse = zero_vec;
	t_angular_impulse = zero_vec;
}

void ServerEntity::calculate(float dt) {
	/*
	D3DXVECTOR3 dv = ((force / mass ) * dt);
	if(D3DXVec3Length(&(velocity + dv)) <= maxVelocity)
		velocity = velocity + dv;
	m_pos = m_pos + velocity;
	*/
}


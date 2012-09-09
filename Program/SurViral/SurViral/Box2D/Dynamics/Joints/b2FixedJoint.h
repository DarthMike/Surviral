#ifndef B2_FIXED_JOINT_H
#define B2_FIXED_JOINT_H

#include "b2Joint.h"

/// FixedJoint: Attaches two bodies rigidly together
struct b2FixedJointDef : public b2JointDef
{
	b2FixedJointDef()
	{
		type = e_fixedJoint;
	}

	/// Initialize the bodies.
	void Initialize(b2Body* body1, b2Body* body2);
};

/// A fixed joint constrains all degrees of freedom between two bodies
/// Author: Jorrit Rouwe
/// See: www.jrouwe.nl/fixedjoint/ for more info
class b2FixedJoint : public b2Joint
{
public:

	b2Vec2 GetReactionForce(float32 inv_dt) const;  
	float32 GetReactionTorque(float32 inv_dt) const;

	b2Vec2 GetAnchor1() const;
	b2Vec2 GetAnchor2() const;

	//--------------- Internals Below -------------------

	b2FixedJoint(const b2FixedJointDef* data);

	void InitVelocityConstraints(const b2TimeStep& step);
	void SolveVelocityConstraints(const b2TimeStep& step);
	bool SolvePositionConstraints(float32 baumgarte);

private:
	// Get effective constraint mass
	void CalculateMC();

	// Configured state for bodies
	b2Vec2 m_dp;		//< Distance between body->GetXForm().position between the two bodies at rest in the reference frame of body1
	float32 m_a;		//< Angle between the bodies at rest
	b2Mat22 m_R0;		//< Rotation matrix of m_a
	
	// State for solving
	float32 m_inv_dt;	//< Stored 1/dt
	b2Vec2 m_d;			//< Distance between center of masses for this time step (when the shapes of the bodies change, their local centers can change so we derive this from m_dp every frame)
	float m_a1;			//< Stored angle of body 1 (a1) to determine if it changed
	float m_c, m_s;		//< cos(a1) and sin(a1)
	float m_Ax, m_Ay;	//< A = d/dt (R(a1) d)
	float m_mc[3][3];	//< Effective constraint mass

	// State after solving
	float m_lambda[3];	//< Accumulated lambdas for warm starting and returning constraint force
};

#endif

#include "b2FixedJoint.h"
#include "../b2Body.h"
#include "../b2World.h"

void b2FixedJointDef::Initialize(b2Body* b1, b2Body* b2)
{
	body1 = b1;
	body2 = b2;
}

b2FixedJoint::b2FixedJoint(const b2FixedJointDef* def)
	: b2Joint(def)
{
	// Get bodies
	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	// Get initial delta position and angle
	m_dp = b2MulT(b1->GetXForm().R, b2->GetXForm().position - b1->GetXForm().position);
	m_a = b2->GetAngle() - b1->GetAngle();
	m_R0 = b2MulT(b1->GetXForm().R, b2->GetXForm().R);

	// Reset accumulated lambda
	m_lambda[0] = m_lambda[1] = m_lambda[2] = 0.0f;
}

void b2FixedJoint::InitVelocityConstraints(const b2TimeStep& step)
{
	// Get bodies
	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	// Store step
	m_inv_dt = step.inv_dt;

	// Get d for this step (transform from delta between positions to delta between center of masses)
	m_d = m_dp - b1->m_sweep.localCenter + b2Mul(m_R0, b2->m_sweep.localCenter);

	// Calculate effective joint mass (stays constant during velocity solving)
	CalculateMC();

	if (step.warmStarting)
	{
		// Apply initial impulse
		b1->m_linearVelocity.x -= b1->m_invMass * m_lambda[0];
		b1->m_linearVelocity.y -= b1->m_invMass * m_lambda[1];
		b1->m_angularVelocity -= b1->m_invI * (m_lambda[0]*m_Ax+m_lambda[1]*m_Ay+m_lambda[2]);
		b2->m_linearVelocity.x += b2->m_invMass * m_lambda[0];
		b2->m_linearVelocity.y += b2->m_invMass * m_lambda[1];
		b2->m_angularVelocity += b2->m_invI * m_lambda[2];
	}
	else
	{
		// Reset accumulated lambda
		m_lambda[0] = m_lambda[1] = m_lambda[2] = 0.0f;
	}
}

void b2FixedJoint::CalculateMC()
{
	// Get bodies
	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	// Cache angle of body 1
	m_a1 = b1->m_sweep.a;

	// Cache sine and cosine of that angle
	m_s = sin(m_a1), m_c = cos(m_a1);

	// Calculate vector A w1 = d/dt (R(a1) d)
	m_Ax = -m_s * m_d.x - m_c * m_d.y;
	m_Ay = m_c * m_d.x - m_s * m_d.y;

	// Calculate effective constraint mass: mC = (J M^-1 J^T)^-1
	float invM1 = b1->m_invMass;
	float invM2 = b2->m_invMass;
	float invM12 = invM1+invM2;
	float invI1 = b1->m_invI;
	float invI2 = b2->m_invI;
	float a = invM12+m_Ax*m_Ax*invI1;
	float b = m_Ax*m_Ay*invI1;
	float c = m_Ax*invI1;
	float d = invM12+m_Ay*m_Ay*invI1;
	float e = m_Ay*invI1;
	float f = invI2+invI1;
	float c1 = d*f-e*e;
	float c2 = c*e-b*f;
	float c3 = b*e-c*d;
	float den = a*c1+b*c2+c*c3;
	m_mc[0][0] = c1/den;
	m_mc[1][0] = c2/den;
	m_mc[2][0] = c3/den;
	m_mc[0][1] = m_mc[1][0];
	m_mc[1][1] = (a*f-c*c)/den;
	m_mc[2][1] = (b*c-a*e)/den;
	m_mc[0][2] = m_mc[2][0];
	m_mc[1][2] = m_mc[2][1];
	m_mc[2][2] = (a*d-b*b)/den;
}

void b2FixedJoint::SolveVelocityConstraints(const b2TimeStep& step)
{
	B2_NOT_USED(step);

	// Get bodies
	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	// Assert that angle is still the same so the effective joint mass is still valid
	assert(b1->m_sweep.a == m_a1);

	// Calculate Cdot
	float Cdot[3] =
	{
		b2->m_linearVelocity.x - b1->m_linearVelocity.x - m_Ax * b1->m_angularVelocity,
		b2->m_linearVelocity.y - b1->m_linearVelocity.y - m_Ay * b1->m_angularVelocity,
		b2->m_angularVelocity - b1->m_angularVelocity
	};

	// Calculate lambda
	float lambda[3];
	for (int r = 0; r < 3; ++r)
		lambda[r] = -(m_mc[r][0] * Cdot[0] + m_mc[r][1] * Cdot[1] + m_mc[r][2] * Cdot[2]);

	// Apply impulse
	b1->m_linearVelocity.x -= b1->m_invMass * lambda[0];
	b1->m_linearVelocity.y -= b1->m_invMass * lambda[1];
	b1->m_angularVelocity -= b1->m_invI * (lambda[0]*m_Ax+lambda[1]*m_Ay+lambda[2]);
	b2->m_linearVelocity.x += b2->m_invMass * lambda[0];
	b2->m_linearVelocity.y += b2->m_invMass * lambda[1];
	b2->m_angularVelocity += b2->m_invI * lambda[2];

	// Accumulate total lambda
	for (int i = 0; i < 3; ++i)
		m_lambda[i] += lambda[i];
}

bool b2FixedJoint::SolvePositionConstraints(float32 baumgarte)
{
	B2_NOT_USED(baumgarte);

	// Get bodies
	b2Body* b1 = m_body1;
	b2Body* b2 = m_body2;

	// Recalculate effective constraint mass if angle changed enough
	if (b2Abs(b1->m_sweep.a - m_a1) > 1e-3f)
		CalculateMC();

	// Calculate C
	float C[3] =
	{
		b2->m_sweep.c.x - b1->m_sweep.c.x - m_c * m_d.x + m_s * m_d.y,
		b2->m_sweep.c.y - b1->m_sweep.c.y - m_s * m_d.x - m_c * m_d.y,
		b2->m_sweep.a - m_a1 - m_a
	};

	// Calculate lambda
	float lambda[3];
	for (int r = 0; r < 3; ++r)
		lambda[r] = -(m_mc[r][0] * C[0] + m_mc[r][1] * C[1] + m_mc[r][2] * C[2]);

	// Apply impulse
	b1->m_sweep.c.x -= b1->m_invMass * lambda[0];
	b1->m_sweep.c.y -= b1->m_invMass * lambda[1];
	b1->m_sweep.a -= b1->m_invI * (lambda[0]*m_Ax+lambda[1]*m_Ay+lambda[2]);
	b2->m_sweep.c.x += b2->m_invMass * lambda[0];
	b2->m_sweep.c.y += b2->m_invMass * lambda[1];
	b2->m_sweep.a += b2->m_invI * lambda[2];

	// Push the changes to the transforms
	b1->SynchronizeTransform();
	b2->SynchronizeTransform();

	// Constraint is satisfied if all constraint equations are nearly zero
	return b2Abs(C[0]) < b2_linearSlop && b2Abs(C[1]) < b2_linearSlop && b2Abs(C[2]) < b2_angularSlop;
}

b2Vec2 b2FixedJoint::GetAnchor1() const 
{ 
	// Return arbitrary position (we have to implement this abstract virtual function)
	return m_body1->GetWorldCenter();
}

b2Vec2 b2FixedJoint::GetAnchor2() const 
{ 
	// Return arbitrary position (we have to implement this abstract virtual function)
	return m_body2->GetWorldCenter(); 
}

b2Vec2 b2FixedJoint::GetReactionForce(float32 inv_dt) const
{
	return inv_dt * b2Vec2(m_lambda[0], m_lambda[1]);
}

float32 b2FixedJoint::GetReactionTorque(float32 inv_dt) const
{
	return inv_dt * m_lambda[2];
}

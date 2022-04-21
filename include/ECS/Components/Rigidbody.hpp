#ifndef ELYS_RIGIDBODY_HPP
#define ELYS_RIGIDBODY_HPP

#include "glm/glm.hpp"

namespace Elys {
	class Rigidbody {
	public:
		Rigidbody();

		void Update();

	private:
		float mMass = 1;
		float mDrag = 0;
		float mAngularDrag = 0;
		bool mUseGravity = true;
		bool mIsKinematic = false;
		bool mConstraintPosition[3] = {false,false,false};
		bool mConstraintRotation[3] = {false,false,false};

		float mSpeed;
		glm::vec3 mVelocity;
		glm::vec3 mAngularVelocity;
		glm::vec3 mInertiaTensor;
		glm::vec3 mInertiaTensorRotation;
		glm::vec3 mLocalMassCenter;
		glm::vec3 mWorldMassCenter;
	};
}

#endif
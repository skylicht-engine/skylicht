#include "pch.h"
#include "CIKHand.h"
#include "GameObject/CGameObject.h"
#include "Entity/CEntityManager.h"
#include "Transform/CWorldTransformSystem.h"

CIKHand::CIKHand() :
	m_gun(NULL),
	m_aimWeight(0.0f),
	m_enable(true),
	m_drawSekeleton(false),
	m_skeleton(NULL)
{

}

CIKHand::~CIKHand()
{
	CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	transformSystem->unRegisterLateUpdate(this);
}

void CIKHand::initComponent()
{
	CWorldTransformSystem* transformSystem = m_gameObject->getEntityManager()->getSystem<CWorldTransformSystem>();
	transformSystem->registerLateUpdate(this);
}

void CIKHand::updateComponent()
{
	if (!m_enable)
		return;

	// notify late update for hand nodes
	for (int i = 0; i < 3; i++)
	{
		notifyLateUpdate(m_leftHand.Nodes[i]);
		notifyLateUpdate(m_rightHand.Nodes[i]);
	}
}

void CIKHand::notifyLateUpdate(CAnimationTransformData* node)
{
	node->WorldTransform->HasLateChanged = true;
}

void CIKHand::setGun(CAnimationTransformData* gun)
{
	m_gun = gun;
}

void CIKHand::setLeftHand(CAnimationTransformData* start, CAnimationTransformData* mid, CAnimationTransformData* end)
{
	m_leftHand.Nodes[0] = start;
	m_leftHand.Nodes[1] = mid;
	m_leftHand.Nodes[2] = end;
}

void CIKHand::setRightHand(CAnimationTransformData* start, CAnimationTransformData* mid, CAnimationTransformData* end)
{
	m_rightHand.Nodes[0] = start;
	m_rightHand.Nodes[1] = mid;
	m_rightHand.Nodes[2] = end;
}

core::vector3df CIKHand::getWorldPosition(CAnimationTransformData* node)
{
	return node->WorldTransform->World.getTranslation();
}

const core::matrix4& CIKHand::getWorldTransform(CAnimationTransformData* node)
{
	return node->WorldTransform->World;
}

void CIKHand::lateUpdate()
{
	if (!m_enable)
		return;

	CSceneDebug* debug = CSceneDebug::getInstance()->getNoZDebug();

	core::vector3df left[3];
	core::vector3df right[3];

	for (int i = 0; i < 3; i++)
	{
		left[i] = getWorldPosition(m_leftHand.Nodes[i]);
		right[i] = getWorldPosition(m_rightHand.Nodes[i]);
	}

	core::vector3df aimCenter = (right[0] + left[0]) * 0.5f;

	if (m_gun)
	{
		const core::matrix4& gunWorld = getWorldTransform(m_gun);

		core::vector3df gunFront(1.0f, 0.0f, 0.0f);
		gunWorld.rotateVect(gunFront);
		gunFront.normalize();

		core::vector3df gunPos = getWorldPosition(m_gun);
		core::vector3df gunVec = gunPos - aimCenter;

		// core::vector3df gunHandOffset = gunPos - right[2];
		core::vector3df gunHandOffset(-0.0254051760, 0.107825637, 0.114578426); // <- by default animation

		// get aim vector
		core::vector3df aimVec = m_aimTarget - aimCenter;
		aimVec.normalize();

		debug->addLine(aimCenter, aimCenter + aimVec * 2.0f, SColor(255, 0, 0, 100));

		// predict gun position by aim vector
		core::vector3df predictGunPos = aimCenter + aimVec * gunVec.getLength();

		// core::vector3df gunAdjustOffset = predictGunPos - gunPos;
		core::vector3df gunAdjustOffset(-0.0412531942f, -0.0346513987f, 0.00291103125f); // <- by default animation

		// offset for vector (0.0, 0.0, 1.0)
		core::vector3df offset = gunHandOffset + gunAdjustOffset;

		// rotate offset for aim vector
		core::vector3df upVec(0.0f, 1.0f, 0.0f);
		core::vector3df rightVec = upVec.crossProduct(aimVec);
		upVec = aimVec.crossProduct(rightVec);

		core::matrix4 rotationMatrix;
		f32* matData = rotationMatrix.pointer();

		matData[0] = rightVec.X;
		matData[1] = rightVec.Y;
		matData[2] = rightVec.Z;
		matData[3] = 0.0f;

		matData[4] = upVec.X;
		matData[5] = upVec.Y;
		matData[6] = upVec.Z;
		matData[7] = 0.0f;

		matData[8] = aimVec.X;
		matData[9] = aimVec.Y;
		matData[10] = aimVec.Z;
		matData[11] = 0.0f;

		matData[12] = 0.0f;
		matData[13] = 0.0f;
		matData[14] = 0.0f;
		matData[15] = 1.0f;

		rotationMatrix.rotateVect(offset);

		core::vector3df predictRHandPos = predictGunPos - offset;

		// draw debug
		if (m_drawSekeleton)
		{
			debug->addSphere(predictGunPos, 0.02f, SColor(255, 255, 255, 0));
			debug->addSphere(predictRHandPos, 0.02f, SColor(255, 0, 255, 255));
		}

		// blending by aim ratio
		predictRHandPos = right[2] * (1.0f - m_aimWeight) + predictRHandPos * m_aimWeight;

		// transform to local hand position
		core::matrix4 rightHandWorldInv = getWorldTransform(m_rightHand.Nodes[0]);
		rightHandWorldInv.makeInverse();

		core::matrix4 leftHandWorldInv = getWorldTransform(m_leftHand.Nodes[0]);
		leftHandWorldInv.makeInverse();

		for (int i = 0; i < 3; i++)
		{
			rightHandWorldInv.transformVect(right[i]);
			leftHandWorldInv.transformVect(left[i]);
		}

		core::vector3df rightPole = getPoleVector(right);
		rightPole.normalize();

		core::vector3df leftPole = getPoleVector(left);
		leftPole.normalize();

		core::vector3df targetRight;
		rightHandWorldInv.transformVect(targetRight, predictRHandPos);

		// solve ik in right hand
		core::quaternion qStart, qMid;
		m_ikRightHand.solveIK(right[0], right[1], right[2], targetRight, rightPole, 0.1f, qStart, qMid);

		m_rightHand.Nodes[0]->AnimRotation *= qStart;
		m_rightHand.Nodes[0]->updateTransform();

		m_rightHand.Nodes[1]->AnimRotation *= qMid;
		m_rightHand.Nodes[1]->updateTransform();
	}

	if (m_skeleton && m_drawSekeleton)
		m_skeleton->drawDebug(core::IdentityMatrix, SColor(100, 255, 255, 255));
}

core::vector3df CIKHand::getPoleVector(const core::vector3df* p)
{
	core::vector3df pole = p[1] - p[0];
	core::vector3df endVec = p[2] - p[0];

	core::vector3df norm = pole.crossProduct(endVec);
	norm.normalize();

	core::quaternion q;
	q.fromAngleAxis(-45.0f * core::DEGTORAD, norm);

	pole = q * pole;
	return pole;
}
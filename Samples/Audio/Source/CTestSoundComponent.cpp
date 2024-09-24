#include "pch.h"
#include "CTestSoundComponent.h"
#include "GameObject/CGameObject.h"

CTestSoundComponent::CTestSoundComponent() :
	m_sprite(NULL),
	m_frame(NULL),
	m_sound(NULL),
	m_angle(90.0f),
	m_radius(5.0f)
{

}

CTestSoundComponent::~CTestSoundComponent()
{
	m_sprite->drop();
}

void CTestSoundComponent::setFrame(CSpriteAtlas* sprite, SFrame* frame)
{
	m_sprite = sprite;
	m_frame = frame;

	m_sprite->grab();

	CSprite::setFrame(m_frame, 0.005f, SColor(255, 255, 255, 255));
	setBillboard(true);
	setCenter(true);
}

void CTestSoundComponent::initComponent()
{
	CSprite::initComponent();
}

void CTestSoundComponent::updateComponent()
{
	CSprite::updateComponent();

	m_angle = m_angle + getTimeStep() * 0.0002f;
	m_angle = fmodf(m_angle, 360.0f);

	core::vector3df pos;
	pos.X = m_radius * sinf(m_angle);
	pos.Z = m_radius * cosf(m_angle);

	// object position
	m_gameObject->getTransformEuler()->setPosition(pos);

	// sound position
	m_sound->setPosition(pos.X, pos.Y, pos.Z);
}
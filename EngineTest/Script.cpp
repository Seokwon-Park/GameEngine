#include "Components/Entity.h"
#include "Components/Transform.h"
#include "Components/Script.h"

using namespace primal;

class rotator_script;
REGISTER_SCRIPT(rotator_script);
class rotator_script : public script::entity_script
{
public:
	constexpr explicit rotator_script(game_entity::entity entity)
		:script::entity_script{ entity }{}

	void begin_play() override {}
	void update(float dt) override
	{
		_angle += 0.25f * dt * math::two_pi;
		if (_angle > math::two_pi) _angle -= math::two_pi;
		math::v3a rot{ 0.f, _angle, 0.f };
		DirectX::XMVECTOR quat{ DirectX::XMQuaternionRotationRollPitchYawFromVector(DirectX::XMLoadFloat3A(&rot)) };
		math::v4 rot_quat{};
		DirectX::XMStoreFloat4(&rot_quat, quat);
		set_rotation(rot_quat);
	}

private:
	f32 _angle{ 0.f };
};
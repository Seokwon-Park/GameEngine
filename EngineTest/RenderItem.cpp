#include <filesystem>
#include "CommonHeaders.h"
#include "Content/ContentToEngine.h"
#include "ShaderCompilation.h"

using namespace primal;

bool read_file(std::filesystem::path, std::unique_ptr<u8[]>&, u64&);

namespace
{
	id::id_type model_id{ id::invalid_id };
	id::id_type vs_id{ id::invalid_id };
	id::id_type ps_id{ id::invalid_id };

	void load_model()
	{
		std::unique_ptr<u8[]> model;
		u64 size{ 0 };
		read_file("..\\..\\enginetest\\model.model", model, size);

		model_id = content::create_resource(model.get(), content::asset_type::mesh);
		assert(id::is_valid(model_id));
	}

	void load_shaders()
	{
		// VS�� PS�� Material�� ����Ѵٰ� ����
	}
} // nonoymouse namespace

id::id_type create_render_item(id::id_type entity_id)
{
	// load a model, pretend it belongs to entity_id
	auto _1 = std::thread{ [] {load_model(); } };

	//load material:
	// 1) load textures, oh nooooo we don't have any, but that's ok.
	// 2) load shaders for that material
	auto _2 = std::thread{ [] {load_shaders(); } };

	_1.join();
	_2.join();
	// add a render item using the model and its materials.

	return id::invalid_id;
}

void destroy_render_item(id::id_type item_id)
{

}
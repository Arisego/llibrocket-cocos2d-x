#ifndef __ROCKET_GLU_H__
#define __ROCKET_GLU_H__


#include <Rocket/Core.h>
#include <Rocket/Debugger.h>
#include "cocos2d.h"

class CocoSystemInterface : public Rocket::Core::SystemInterface
{
private:
	struct cocos2d::cc_timeval begin;
public:
	virtual float GetElapsedTime(){
		struct cocos2d::cc_timeval now;
		cocos2d::CCTime::gettimeofdayCocos2d(&now,NULL);
		return cocos2d::CCTime::timersubCocos2d(&begin,&now)/1000;
	}

	void init(){
		cocos2d::CCTime::gettimeofdayCocos2d(&begin,NULL); 
	}

};


class CocoRenderInterface : public Rocket::Core::RenderInterface
{
private:
	GLuint fragment_texture_shader_id,fragment_color_shader_id,vertex_shader_id;
	GLuint program_texture_id,u_texture,u_texture_projection,u_texture_translation,program_color_id,u_color_projection,u_color_translation;
public:

	CocoRenderInterface();

	/// Called by Rocket when it wants to render geometry that it does not wish to optimise.
	virtual void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation);

	/// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
	virtual Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture);

	/// Called by Rocket when it wants to render application-compiled geometry.
	virtual void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation);
	/// Called by Rocket when it wants to release application-compiled geometry.
	virtual void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

	/// Called by Rocket when it wants to enable or disable scissoring to clip content.
	virtual void EnableScissorRegion(bool enable);
	/// Called by Rocket when it wants to change the scissor region.
	virtual void SetScissorRegion(int x, int y, int width, int height);

	/// Called by Rocket when a texture is required by the library.
	virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
	/// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
	virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);
	/// Called by Rocket when a loaded texture is no longer required.
	virtual void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);
};

#endif
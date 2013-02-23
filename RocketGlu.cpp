#include "RocketGlu.h"

#define GL_CLAMP_TO_EDGE 0x812F

typedef enum {
	ROCKETGLUE_ATTRIBUTE_POSITION = 0,
	ROCKETGLUE_ATTRIBUTE_TEXCOORD = 1,
	ROCKETGLUE_ATTRIBUTE_COLOR = 2
} ROCKETGLUE_ATTRIBUTES;

typedef enum
{
	ROCKETGLUE_UNIFORM_PROJECTION = 0,
	ROCKETGLUE_UNIFORM_TEXTURE = 1,
	ROCKETGLUE_UNIFORM_TRANSLATION = 0,
} ROCKETGLUE_UNIFORM;

static const char RocketGlueFragmentTextureShader[] = " \
													  precision mediump float; \
													  varying vec2 v_texCoord; \
													  varying vec4 v_color; \
													  uniform sampler2D u_texture; \
													  \
													  void main() \
													  { \
													  gl_FragColor = texture2D( u_texture, v_texCoord ) * v_color; \
													  } \
													  ";

static const char RocketGlueFragmentColorShader[] = " \
													precision mediump float; \
													varying vec2 v_texCoord; \
													varying vec4 v_color; \
													\
													void main() \
													{ \
													gl_FragColor = v_color; \
													} \
													";

static const char RocketGlueVertexShader[] = " \
											 attribute vec2 a_position; \
											 attribute vec2 a_texCoord; \
											 attribute vec4 a_color; \
											 varying vec2 v_texCoord; \
											 varying vec4 v_color; \
											 varying vec2 v_position; \
											 uniform mat4 u_projection; \
											 uniform vec2 u_translation; \
											 \
											 void main() \
											 { \
											 v_position = a_position + u_translation; \
											 gl_Position = u_projection * vec4(v_position, 0.0, 1.0); \
											 v_texCoord = a_texCoord; \
											 v_color = a_color; \
											 }\
											 ";



CocoRenderInterface::CocoRenderInterface()
{

	glGetError();

	/* Create shaders */
	GLint compileSuccessful = GL_FALSE, linkSuccessful = GL_FALSE;
	GLenum glerr;

	/* Textured fragment shader */
	fragment_texture_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_texture_source[1] = {RocketGlueFragmentTextureShader};
	const GLint fragment_texture_len[1] = {(GLint)sizeof(RocketGlueFragmentTextureShader)-1};
	glShaderSource(fragment_texture_shader_id, 1, fragment_texture_source, fragment_texture_len);
	glCompileShader(fragment_texture_shader_id);
	glGetShaderiv(fragment_texture_shader_id, GL_COMPILE_STATUS, &compileSuccessful);

	glerr = glGetError();
	if( glerr != GL_NO_ERROR || ! compileSuccessful) {
		char errorLog[4096];
		int length;
		glGetShaderInfoLog( fragment_texture_shader_id, 4096, &length, errorLog);
		exit(1);
		return;
	}

	/* Colored fragment shader */
	compileSuccessful = GL_FALSE;
	fragment_color_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_color_source[1] = {RocketGlueFragmentColorShader};
	const GLint fragment_color_len[1] = {(GLint)sizeof(RocketGlueFragmentColorShader)-1};
	glShaderSource(fragment_color_shader_id, 1, fragment_color_source, fragment_color_len);
	glCompileShader(fragment_color_shader_id);
	glGetShaderiv(fragment_color_shader_id, GL_COMPILE_STATUS, &compileSuccessful);

	glerr = glGetError();
	if( glerr != GL_NO_ERROR || ! compileSuccessful) {
		char errorLog[4096];
		int length;
		glGetShaderInfoLog( fragment_color_shader_id, 4096, &length, errorLog);
		exit(1);
		return;
	}

	compileSuccessful = GL_FALSE;
	vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_source[1] = {RocketGlueVertexShader};
	const GLint vertex_len[1] = {(GLint)sizeof(RocketGlueVertexShader)-1};
	glShaderSource(vertex_shader_id, 1, vertex_source, vertex_len);
	glCompileShader(vertex_shader_id);
	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &compileSuccessful);

	glerr = glGetError();
	if( glerr != GL_NO_ERROR || ! compileSuccessful) {
		char errorLog[4096];
		int length;
		glGetShaderInfoLog( vertex_shader_id, 4096, &length, errorLog);
		exit(1);
		return;
	}

	/* Create Programs */

	program_texture_id = glCreateProgram();
	glAttachShader(program_texture_id, vertex_shader_id);
	glAttachShader(program_texture_id, fragment_texture_shader_id);
	glBindAttribLocation(program_texture_id, ROCKETGLUE_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(program_texture_id, ROCKETGLUE_ATTRIBUTE_TEXCOORD, "a_texCoord");
	glBindAttribLocation(program_texture_id, ROCKETGLUE_ATTRIBUTE_COLOR, "a_color");
	glLinkProgram(program_texture_id);
	glGetProgramiv(program_texture_id, GL_LINK_STATUS, &linkSuccessful);

	glerr = glGetError();
	if ( glerr != GL_NO_ERROR || !linkSuccessful)
	{
		char errorLog[4096] = "";
		int length;
		glGetProgramInfoLog( program_texture_id, 4096, &length, errorLog);
		exit(1);
		return;
	}
	
	u_texture = glGetUniformLocation(program_texture_id, "u_texture");
	u_texture_projection = glGetUniformLocation(program_texture_id, "u_projection");
	u_texture_translation = glGetUniformLocation(program_texture_id, "u_translation");

	program_color_id = glCreateProgram();
	glAttachShader(program_color_id, vertex_shader_id);
	glAttachShader(program_color_id, fragment_color_shader_id);
	glBindAttribLocation(program_color_id, ROCKETGLUE_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(program_color_id, ROCKETGLUE_ATTRIBUTE_TEXCOORD, "a_texCoord");
	glBindAttribLocation(program_color_id, ROCKETGLUE_ATTRIBUTE_COLOR, "a_color");
	glLinkProgram(program_color_id);
	glGetProgramiv(program_color_id, GL_LINK_STATUS, &linkSuccessful);

	glerr = glGetError();
	if ( glerr != GL_NO_ERROR || !linkSuccessful)
	{
		char errorLog[4096] = "";
		int length;
		glGetProgramInfoLog( program_color_id, 4096, &length, errorLog);
		exit(1);
		return;
	}

	u_color_projection = glGetUniformLocation(program_color_id, "u_projection");
	u_color_translation = glGetUniformLocation(program_color_id, "u_translation");

}

// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void CocoRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
    if(texture) glUseProgram(program_texture_id);
    else glUseProgram(program_color_id);
	
	glDisable(GL_DEPTH_TEST);

	cocos2d::CCSize rvp = cocos2d::CCDirector::sharedDirector()->getVisibleSize();

    GLfloat projection[4][4];

    // Prepare an orthographic projection
    projection[0][0] = 2.0f / rvp.width;
    projection[0][1] = 0.0f;
    projection[0][2] = 0.0f;
    projection[0][3] = 0.0f;
    projection[1][0] = 0.0f;
    //if (renderer->target) {
    //    projection[1][1] = 2.0f / height;
    //} else {
        projection[1][1] = -2.0f / rvp.height;
    //}
    projection[1][2] = 0.0f;
    projection[1][3] = 0.0f;
    projection[2][0] = 0.0f;
    projection[2][1] = 0.0f;
    projection[2][2] = 0.0f;
    projection[2][3] = 0.0f;
    projection[3][0] = -1.0f;
    //if (renderer->target) {
    //    projection[3][1] = -1.0f;
    //} else {
        projection[3][1] = 1.0f;
    //}
    projection[3][2] = 0.0f;
    projection[3][3] = 1.0f;

    // Set the projection matrix
    if (texture) {
        glUniformMatrix4fv(u_texture_projection, 1, GL_FALSE, (GLfloat *)projection);
        glUniform2f(u_texture_translation, translation.x, translation.y);
    }
    else {
        glUniformMatrix4fv(u_color_projection, 1, GL_FALSE, (GLfloat *)projection);
        glUniform2f(u_color_translation, translation.x, translation.y);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    unsigned short* newIndicies;
	newIndicies = new unsigned short[num_indices];
    for (int i = 0; i < num_indices; i++)
    {
      newIndicies[i] = (unsigned short) indices[i];
    }

    glVertexAttribPointer(ROCKETGLUE_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Rocket::Core::Vertex), &vertices[0].position);
    glVertexAttribPointer(ROCKETGLUE_ATTRIBUTE_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Rocket::Core::Vertex), &vertices[0].colour);
    glEnableVertexAttribArray(ROCKETGLUE_ATTRIBUTE_POSITION);
    glEnableVertexAttribArray(ROCKETGLUE_ATTRIBUTE_TEXCOORD);
    glEnableVertexAttribArray(ROCKETGLUE_ATTRIBUTE_COLOR);

    if(texture) {
        glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
        glUniform1i(u_texture, 0);
        glVertexAttribPointer(ROCKETGLUE_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Rocket::Core::Vertex), &vertices[0].tex_coord);
    }
    else {
        glActiveTexture(GL_TEXTURE0);
        //glDisable(GL_TEXTURE_2D);
        //glDisableVertexAttribArray(ROCKETGLUE_ATTRIBUTE_TEXCOORD);
    }

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, newIndicies);

    /* We can disable ROCKETGLUE_ATTRIBUTE_COLOR (2) safely as SDL will reenable the vertex attrib 2 if it is required */
    //glDisableVertexAttribArray(ROCKETGLUE_ATTRIBUTE_COLOR);

    /* Leave ROCKETGLUE_ATTRIBUTE_POSITION (0) and ROCKETGLUE_ATTRIBUTE_TEXCOORD (1) enabled for compatibility with SDL which
       doesn't re enable them when you call RenderCopy/Ex */
    if(!texture) 
		glEnableVertexAttribArray(ROCKETGLUE_ATTRIBUTE_TEXCOORD);

    /* Reset blending and draw a fake point just outside the screen to let SDL know that it needs to reset its state in case it wants to render a texture */
    //glDisable(GL_BLEND);
	glPopAttrib();

}

// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.		
Rocket::Core::CompiledGeometryHandle CocoRenderInterface::CompileGeometry(Rocket::Core::Vertex* ROCKET_UNUSED(vertices), int ROCKET_UNUSED(num_vertices), int* ROCKET_UNUSED(indices), int ROCKET_UNUSED(num_indices), const Rocket::Core::TextureHandle ROCKET_UNUSED(texture))
{
	return (Rocket::Core::CompiledGeometryHandle) NULL;
}

// Called by Rocket when it wants to render application-compiled geometry.		
void CocoRenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry), const Rocket::Core::Vector2f& ROCKET_UNUSED(translation))
{
}

// Called by Rocket when it wants to release application-compiled geometry.		
void CocoRenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry))
{
}

// Called by Rocket when it wants to enable or disable scissoring to clip content.		
void CocoRenderInterface::EnableScissorRegion(bool enable)
{
	if (enable)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);
}

// Called by Rocket when it wants to change the scissor region.		
void CocoRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
	glScissor(x, cocos2d::CCDirector::sharedDirector()->getVisibleSize().height - (y + height), width, height);
}

// Set to byte packing, or the compiler will expand our struct, which means it won't read correctly from file
#pragma pack(1) 
struct TGAHeader 
{
	char  idLength;
	char  colourMapType;
	char  dataType;
	short int colourMapOrigin;
	short int colourMapLength;
	char  colourMapDepth;
	short int xOrigin;
	short int yOrigin;
	short int width;
	short int height;
	char  bitsPerPixel;
	char  imageDescriptor;
};
// Restore packing
#pragma pack()

// Called by Rocket when a texture is required by the library.		
bool CocoRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
	Rocket::Core::FileInterface* file_interface = Rocket::Core::GetFileInterface();
	Rocket::Core::FileHandle file_handle = file_interface->Open(source);
	if (!file_handle)
	{
		return false;
	}

	file_interface->Seek(file_handle, 0, SEEK_END);
	size_t buffer_size = file_interface->Tell(file_handle);
	file_interface->Seek(file_handle, 0, SEEK_SET);

	char* buffer = new char[buffer_size];
	file_interface->Read(buffer, buffer_size, file_handle);
	file_interface->Close(file_handle);

	TGAHeader header;
	memcpy(&header, buffer, sizeof(TGAHeader));

	int color_mode = header.bitsPerPixel / 8;
	int image_size = header.width * header.height * 4; // We always make 32bit textures 

	if (header.dataType != 2)
	{
		Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Only 24/32bit uncompressed TGAs are supported.");
		return false;
	}

	// Ensure we have at least 3 colors
	if (color_mode < 3)
	{
		Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Only 24 and 32bit textures are supported");
		return false;
	}

	const char* image_src = buffer + sizeof(TGAHeader);
	unsigned char* image_dest = new unsigned char[image_size];

	// Targa is BGR, swap to RGB and flip Y axis
	for (long y = 0; y < header.height; y++)
	{
		long read_index = y * header.width * color_mode;
		long write_index = ((header.imageDescriptor & 32) != 0) ? read_index : (header.height - y - 1) * header.width * color_mode;
		for (long x = 0; x < header.width; x++)
		{
			image_dest[write_index] = image_src[read_index+2];
			image_dest[write_index+1] = image_src[read_index+1];
			image_dest[write_index+2] = image_src[read_index];
			if (color_mode == 4)
				image_dest[write_index+3] = image_src[read_index+3];
			else
				image_dest[write_index+3] = 255;

			write_index += 4;
			read_index += color_mode;
		}
	}

	texture_dimensions.x = header.width;
	texture_dimensions.y = header.height;

	bool success = GenerateTexture(texture_handle, image_dest, texture_dimensions);

	delete [] image_dest;
	delete [] buffer;

	return success;
}

// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
bool CocoRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	if (texture_id == 0)
	{
		printf("Failed to generate textures\n");
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	texture_handle = (Rocket::Core::TextureHandle) texture_id;

	return true;
}

// Called by Rocket when a loaded texture is no longer required.		
void CocoRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
	glDeleteTextures(1, (GLuint*) &texture_handle);
}




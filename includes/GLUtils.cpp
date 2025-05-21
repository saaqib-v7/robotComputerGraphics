#include "GLUtils.hpp"

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <SDL2/SDL_image.h>

/* 

 Based on http://www.opengl-tutorial.org/

*/

static void loadShaderCode( std::string& shaderCode, const std::filesystem::path& _fileName )
{
	// loading in shader code from the _fileName file
	shaderCode = "";

	// opening _fileName
	std::ifstream shaderStream( _fileName );
	if ( !shaderStream.is_open() )
	{
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR,
						SDL_LOG_PRIORITY_ERROR,
						"Error while opening shader code file %s!", _fileName.string().c_str());
		return;
	}

	// loading the file's contents into shaderCode
	std::string line = "";
	while ( std::getline( shaderStream, line ) )
	{
		shaderCode += line + "\n";
	}

	shaderStream.close();
}

GLuint AttachShader( const GLuint programID, GLenum shaderType, const std::filesystem::path& _fileName )
{
	// loading in shader code from the _fileName file
    std::string shaderCode;
    loadShaderCode( shaderCode, _fileName );

    return AttachShaderCode( programID, shaderType, shaderCode );
}

GLuint AttachShaderCode( const GLuint programID, GLenum shaderType, std::string_view shaderCode )
{
	if (programID == 0)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_ERROR,
						SDL_LOG_PRIORITY_ERROR,
						"Program needs to be inited before loading!");
		return 0;
	}

	// creating the shader
	GLuint shaderID = glCreateShader( shaderType );

	// assigning code to the shader
	const char* sourcePointer = shaderCode.data();
	GLint sourceLength = static_cast<GLint>( shaderCode.length() );

	glShaderSource( shaderID, 1, &sourcePointer, &sourceLength );

	// compiling the shader
	glCompileShader( shaderID );

	// check if everything is okay
	GLint result = GL_FALSE;
	int infoLogLength;

	// getting the status of the compiling
	glGetShaderiv( shaderID, GL_COMPILE_STATUS, &result );
	glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &infoLogLength );

	if ( GL_FALSE == result || infoLogLength != 0 )
	{
		// getting the error message and printing it
		std::string ErrorMessage( infoLogLength, '\0' );
		glGetShaderInfoLog( shaderID, infoLogLength, NULL, ErrorMessage.data() );

		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR,
						( result ) ? SDL_LOG_PRIORITY_WARN : SDL_LOG_PRIORITY_ERROR,
						"[glCompileShader]: %s", ErrorMessage.data() );
	}

	// assigning the shader to the program
	glAttachShader( programID, shaderID );

	return shaderID;

}

void LinkProgram( const GLuint programID, bool OwnShaders )
{
	// link the shaders (linking input and output variables, etc.)
	glLinkProgram( programID );

	// checking the link
	GLint infoLogLength = 0, result = 0;

	glGetProgramiv( programID, GL_LINK_STATUS, &result );
	glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &infoLogLength );
	if ( GL_FALSE == result || infoLogLength != 0 )
	{
		std::string ErrorMessage( infoLogLength, '\0' );
		glGetProgramInfoLog( programID, infoLogLength, nullptr, ErrorMessage.data() );
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR,
						( result ) ? SDL_LOG_PRIORITY_WARN : SDL_LOG_PRIORITY_ERROR,
						"[glLinkProgram]: %s", ErrorMessage.data() );
	}

	// In this case, the shader object is attached to the program object.
	// Which means we can "delete" the shader object.
	// According to the standard (https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDeleteShader.xhtml)
	// the shader objects will only be deleted, when they are no longer attached to any program objects.
	// Therefore, when we delete the program object, the shader objects will be deleted as well.
	if ( OwnShaders )
	{
		// get the shader objects attached to the program object, ...
        GLint attachedShaders = 0;
        glGetProgramiv( programID, GL_ATTACHED_SHADERS, &attachedShaders );
        std::vector<GLuint> shaders( attachedShaders );

        glGetAttachedShaders( programID, attachedShaders, nullptr, shaders.data() );

        // ...  and "delete" them
        for ( GLuint shader : shaders )
        {
            glDeleteShader( shader );
        }

	}
}

static inline ImageRGBA::TexelRGBA* get_image_row( ImageRGBA& image, int rowIndex )
{
	return &image.texelData[  rowIndex * image.width ];
}

static void invert_image_RGBA(ImageRGBA& image)
{
	int height_div_2 = image.height / 2;


	for ( int index = 0; index < height_div_2; index++ )
	{
		std::uint32_t* lower_data  =reinterpret_cast<std::uint32_t*>(get_image_row( image, index) );
		std::uint32_t* higher_data =reinterpret_cast<std::uint32_t*>(get_image_row( image, image.height - 1 - index ) );

		for ( unsigned int rowIndex = 0; rowIndex < image.width; rowIndex++ )
		{
			lower_data[ rowIndex ] ^= higher_data[ rowIndex ];
			higher_data[ rowIndex ] ^= lower_data[ rowIndex ];
			lower_data[ rowIndex ] ^= higher_data[ rowIndex ];
		}
	}
}

GLsizei NumberOfMIPLevels( const ImageRGBA& image )
{
	GLsizei targetlevel = 1;
	unsigned int index = std::max( image.width, image.height );

	while (index >>= 1) ++targetlevel;

	return targetlevel;
}

[[nodiscard]] ImageRGBA ImageFromFile( const std::filesystem::path& fileName, bool needsFlip )
{
	ImageRGBA img;

	// Loading the image
	std::unique_ptr<SDL_Surface, decltype( &SDL_FreeSurface )> loaded_img( IMG_Load( fileName.string().c_str() ), SDL_FreeSurface );
	if ( !loaded_img )
	{
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, 
						SDL_LOG_PRIORITY_ERROR,
						"[ImageFromFile] Error while loading image file: %s", fileName.string().c_str());
		return img;
	}

	// SDL stores colors in Uint32, so the byte order matters here
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	Uint32 format = SDL_PIXELFORMAT_ABGR8888;
#else
	Uint32 format = SDL_PIXELFORMAT_RGBA8888;
#endif

	// Conversion to 32bit RGBA format, if it wasn't already in it
	std::unique_ptr<SDL_Surface, decltype( &SDL_FreeSurface )> formattedSurf( SDL_ConvertSurfaceFormat( loaded_img.get(), format, 0 ), SDL_FreeSurface );

	if (!formattedSurf)
	{
		SDL_LogMessage( SDL_LOG_CATEGORY_ERROR, 
						SDL_LOG_PRIORITY_ERROR,
						"[ImageFromFile] Error while processing texture");
		return img;
	}

	// Re-assign the SDL Surface into the ImageRGBA
	img.Assign( reinterpret_cast<const std::uint32_t*>(formattedSurf->pixels), formattedSurf->w, formattedSurf->h );

	// Conversion from SDL coordinates ( (0,0) upper left corner ) to OpenGL texture coordinates ( (0,0) lower left corner )

	if ( needsFlip ) invert_image_RGBA( img );

	return img;
}

void CleanOGLObject( OGLObject& ObjectGPU )
{
	glDeleteBuffers(1,      &ObjectGPU.vboID);
	ObjectGPU.vboID = 0;
	glDeleteBuffers(1,      &ObjectGPU.iboID);
	ObjectGPU.iboID = 0;
	glDeleteVertexArrays(1, &ObjectGPU.vaoID);
	ObjectGPU.vaoID = 0;
}
